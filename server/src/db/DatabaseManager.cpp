#include "api/ApiDTOs.h"
#include "db/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QFile>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    // DEBUG SOLO EN MEMORIA
    m_db.setDatabaseName(":memory:");
    // m_db.setDatabaseName("cafeteria.db"); // Persistencia real en disco
    if (!m_db.open())
    {
        qWarning() << "No se pudo abrir la base de datos:" << m_db.lastError().text();
    }
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

QSqlDatabase &DatabaseManager::database()
{
    return m_db;
}

bool DatabaseManager::inicializarTablas()
{
    // Esqueleto: solo retorna true por ahora
    QSqlQuery query(m_db);
    // Tabla users
    if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username VARCHAR NOT NULL UNIQUE,
                password_hash VARCHAR,
                role VARCHAR,
                active BOOLEAN DEFAULT 1
            )
        )"))
    {
        qWarning() << "Error creando tabla users:" << query.lastError().text();
        return false;
    }
    // Tabla customers
    if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS customers (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name VARCHAR NOT NULL,
                tax_id VARCHAR UNIQUE,
                address VARCHAR,
                email VARCHAR
            )
        )"))
    {
        qWarning() << "Error creando tabla customers:" << query.lastError().text();
        return false;
    }
    // Tabla products
    if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS products (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name VARCHAR NOT NULL,
                price DOUBLE NOT NULL,
                category VARCHAR,
                parent_id INTEGER,
                active BOOLEAN DEFAULT 1,
                FOREIGN KEY(parent_id) REFERENCES products(id)
            )
        )"))
    {
        qWarning() << "Error creando tabla products:" << query.lastError().text();
        return false;
    }
    // Tabla orders
    if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS orders (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                customer_id INTEGER,
                status VARCHAR NOT NULL,
                discount_strategy_id VARCHAR,
                subtotal DOUBLE,
                discount_amount DOUBLE,
                tax_amount DOUBLE,
                total_final DOUBLE,
                FOREIGN KEY(customer_id) REFERENCES customers(id)
            )
        )"))
    {
        qWarning() << "Error creando tabla orders:" << query.lastError().text();
        return false;
    }
    // Tabla order_items
    if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS order_items (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                order_id INTEGER NOT NULL,
                product_id INTEGER NOT NULL,
                quantity INTEGER NOT NULL,
                unit_price DOUBLE,
                FOREIGN KEY(order_id) REFERENCES orders(id),
                FOREIGN KEY(product_id) REFERENCES products(id)
            )
        )"))
    {
        qWarning() << "Error creando tabla order_items:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::seedData()
{
    QFile file(":/server/db/seed.sql");
    if (!file.exists())
    {
        file.setFileName("../server/db/seed.sql"); // fallback para ejecución desde build
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "No se pudo abrir seed.sql:" << file.errorString();
        return false;
    }
    QString sql = file.readAll();
    file.close();

    QStringList statements = sql.split(';', Qt::SkipEmptyParts);
    QSqlQuery query(m_db);
    for (QString stmt : statements)
    {
        stmt = stmt.trimmed();
        if (stmt.isEmpty())
            continue;
        if (!query.exec(stmt))
        {
            qWarning() << "Error ejecutando seedData SQL:" << query.lastError().text() << "\nQuery:" << stmt;
            return false;
        }
    }
    return true;
}

std::vector<InfoProducto> DatabaseManager::getProductos()
{
    std::vector<InfoProducto> productos;
    QSqlQuery query(m_db);
    if (query.exec("SELECT id, name, price FROM products WHERE active = 1"))
    {
        while (query.next())
        {
            InfoProducto dto;
            dto.id = query.value(0).toInt();
            dto.nombre = query.value(1).toString().toStdString();
            dto.precio = query.value(2).toDouble();
            productos.push_back(dto);
        }
    }
    return productos;
}

bool DatabaseManager::savePedido(const InfoPedido &pedidoDTO)
{
    QSqlDatabase &db = m_db;
    QSqlQuery query(db);
    db.transaction();
    // Insertar en orders
    query.prepare("INSERT INTO orders (id, customer_id, status, total_final) VALUES (?, ?, ?, ?)");
    query.addBindValue(pedidoDTO.id_pedido);
    // Buscar el id del cliente por nombre (simplificado, en real usaría id)
    int customerId = 0;
    if (!pedidoDTO.cliente.empty())
    {
        QSqlQuery q(db);
        q.prepare("SELECT id FROM customers WHERE name = ?");
        q.addBindValue(QString::fromStdString(pedidoDTO.cliente));
        if (q.exec() && q.next())
        {
            customerId = q.value(0).toInt();
        }
    }
    query.addBindValue(customerId);
    query.addBindValue(QString::fromStdString(pedidoDTO.estado));
    query.addBindValue(pedidoDTO.total_final);
    if (!query.exec())
    {
        qWarning() << "Error insertando order (DTO):" << query.lastError().text();
        db.rollback();
        return false;
    }
    int orderId = pedidoDTO.id_pedido;
    // Insertar items
    for (const auto &item : pedidoDTO.items)
    {
        QSqlQuery itemQuery(db);
        itemQuery.prepare("INSERT INTO order_items (order_id, product_id, quantity, unit_price) VALUES (?, ?, ?, ?)");
        itemQuery.addBindValue(orderId);
        // Buscar id del producto por nombre (simplificado)
        int productId = 0;
        QSqlQuery pq(db);
        pq.prepare("SELECT id FROM products WHERE name = ?");
        pq.addBindValue(QString::fromStdString(item.nombreProducto));
        if (pq.exec() && pq.next())
        {
            productId = pq.value(0).toInt();
        }
        itemQuery.addBindValue(productId);
        itemQuery.addBindValue(item.cantidad);
        itemQuery.addBindValue(item.precioUnitario);
        if (!itemQuery.exec())
        {
            qWarning() << "Error insertando order_item (DTO):" << itemQuery.lastError().text();
            db.rollback();
            return false;
        }
    }
    db.commit();
    return true;
}