#include <gtest/gtest.h>
#include "core/SistemaPedidos.h"
#include "db/DatabaseManager.h"
#include "api/ApiDTOs.h"

// Mock simple para MenuCafeteria y Producto si es necesario

TEST(SistemaPedidosTest, InicializarMenuCargaProductosDesdeDB)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.inicializarTablas();
    db.seedData();

    SistemaPedidos sistema;
    // Llamar a la función que inicializa el menú desde la base de datos (a implementar)
    sistema.inicializarMenu();

    // Verificar que los productos del seed están presentes
    const auto& productos = sistema.getProductos();
    ASSERT_FALSE(productos.empty());
    bool encontrado = false;
    for (const auto& p : productos) {
        if (p && p->getNombre() == "Cafe Americano") {
            encontrado = true;
            break;
        }
    }
    EXPECT_TRUE(encontrado);
}
