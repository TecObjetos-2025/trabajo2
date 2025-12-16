#include "db/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(":memory:"); // Cambiar a archivo si se requiere persistencia real
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
    // Esqueleto: solo retorna true por ahora
    return true;
}
