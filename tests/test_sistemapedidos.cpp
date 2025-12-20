#include <gtest/gtest.h>
#include "core/SistemaPedidos.h"
#include "db/DatabaseManager.h"
#include "models/Producto.h"
#include "models/Cliente.h"
#include "models/Pedido.h"
#include "api/ApiDTOs.h"
#include "repositories/ProductRepository.h"
#include "repositories/PedidoRepository.h"
#include "repositories/ClienteRepository.h"
#include <QtSql/QSqlQuery>

// Mock simple para MenuCafeteria y Producto si es necesario

TEST(SistemaPedidosTest, InicializarMenuCargaProductosDesdeDB)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.inicializarTablas();
    db.seedData();

    // Crear y cargar repositorio
    auto repo = std::make_shared<ProductRepository>();
    repo->loadFromDB();
    auto orderRepo = std::make_shared<PedidoRepository>();
    auto clienteRepo = std::make_shared<repos::ClienteRepository>();

    SistemaPedidos sistema(repo, orderRepo, clienteRepo);

    // Verificar que los productos del seed están presentes
    const auto &productos = sistema.getProductos();
    ASSERT_FALSE(productos.empty());
    bool encontrado = false;
    for (const auto &p : productos)
    {
        if (p && p->getNombre() == "Cafe Americano")
        {
            encontrado = true;
            break;
        }
    }
    EXPECT_TRUE(encontrado);
}

TEST(SistemaPedidosTest, FinalizarPedidoPersisteEnDB)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.inicializarTablas();
    db.seedData();

    // Crear y cargar repositorio
    auto repo = std::make_shared<ProductRepository>();
    repo->loadFromDB();
    auto orderRepo = std::make_shared<PedidoRepository>();
    auto clienteRepo = std::make_shared<repos::ClienteRepository>();

    SistemaPedidos sistema(repo, orderRepo, clienteRepo);

    // Crear cliente y producto
    auto cliente = std::make_shared<Cliente>(1, "Carlos Juarez", "12345678");
    sistema.registrarPersona(cliente);
    auto productos = sistema.getProductos();
    ASSERT_FALSE(productos.empty());
    auto producto = productos.front();

    // Crear pedido
    auto pedido = std::make_shared<Pedido>(1, cliente);
    pedido->agregarItem(producto, 2); // 2 unidades del primer producto

    // Simular finalizarPedido (a implementar: debe guardar en DB)
    sistema.finalizarPedido(pedido);

    // Buscar el id real de 'Carlos Juarez' en la tabla customers
    int clienteId = -1;
    {
        QSqlQuery q(db.database());
        q.prepare("SELECT id FROM customers WHERE name = ?");
        q.addBindValue("Carlos Juarez");
        if (q.exec() && q.next())
        {
            clienteId = q.value(0).toInt();
        }
    }
    ASSERT_NE(clienteId, -1);

    // Verificar que el pedido fue insertado en la tabla orders
    QSqlQuery query(db.database());
    ASSERT_TRUE(query.exec(QString("SELECT COUNT(*) FROM orders WHERE customer_id = %1").arg(clienteId)));
    ASSERT_TRUE(query.next());
    int count = query.value(0).toInt();
    EXPECT_GT(count, 0);

    // Verificar que los items fueron insertados en order_items
    ASSERT_TRUE(query.exec("SELECT COUNT(*) FROM order_items"));
    ASSERT_TRUE(query.next());
    int itemsCount = query.value(0).toInt();
    EXPECT_GT(itemsCount, 0);
}
