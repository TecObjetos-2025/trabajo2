#include <gtest/gtest.h>
#include "db/DatabaseManager.h"
#include "api/ApiDTOs.h"

TEST(DatabaseManagerTest, GetProductosDevuelveDatosSeed)
{
    DatabaseManager& db = DatabaseManager::instance();
    db.inicializarTablas();
    db.seedData();
    auto productos = db.getProductos();
    ASSERT_FALSE(productos.empty());
    // Verifica que existan productos conocidos del seed
    bool encontrado = false;
    for (const auto& p : productos) {
        if (p.nombre == "Cafe Americano" && p.precio == 5.50) {
            encontrado = true;
            break;
        }
    }
    EXPECT_TRUE(encontrado);
}
