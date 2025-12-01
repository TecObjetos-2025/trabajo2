#include <gtest/gtest.h>
#include "core/CafeteriaFactory.h"
#include "core/SistemaPedidos.h"
#include "models/Cliente.h"
#include "models/Cocinero.h"
#include "models/Administrador.h"

TEST(FactoryTest, CreateCliente)
{
    auto cliente = CafeteriaFactory::crearCliente(1, "Juan", "555-1234");
    ASSERT_NE(cliente, nullptr);
    EXPECT_EQ(cliente->getId(), 1);
    EXPECT_EQ(cliente->getNombre(), "Juan");
    EXPECT_EQ(cliente->getTelefono(), "555-1234");
}

TEST(FactoryTest, CreateCocinero)
{
    // Incluir un puntero nulo para SistemaPedidos por simplicidad
    SistemaPedidos sistema;
    auto cocinero = CafeteriaFactory::crearCocinero(2, "Chef", "C001", &sistema);

    ASSERT_NE(cocinero, nullptr);
    EXPECT_EQ(cocinero->getId(), 2);
    EXPECT_EQ(cocinero->getNombre(), "Chef");
    EXPECT_NE(dynamic_cast<Cocinero *>(cocinero.get()), nullptr);
}

TEST(FactoryTest, CreateAdministrador)
{
    auto admin = CafeteriaFactory::crearAdministrador(3, "Admin", "A001");
    ASSERT_NE(admin, nullptr);
    EXPECT_EQ(admin->getId(), 3);
    EXPECT_EQ(admin->getNombre(), "Admin");
    EXPECT_NE(dynamic_cast<Administrador *>(admin.get()), nullptr);
}

TEST(FactoryTest, CreatePedido)
{
    auto cliente = CafeteriaFactory::crearCliente(1, "Juan", "555-1234");
    auto pedido = CafeteriaFactory::crearPedido(100, cliente);

    ASSERT_NE(pedido, nullptr);
    EXPECT_EQ(pedido->getId(), 100);
    EXPECT_EQ(pedido->getCliente(), cliente);
}

TEST(FactoryTest, ObtenerEstrategia)
{
    // Asumiendo que lo devuelto es no nulo indica éxito

    auto est1 = CafeteriaFactory::obtenerEstrategia("porcentaje_10");
    ASSERT_NE(est1, nullptr);

    auto est2 = CafeteriaFactory::obtenerEstrategia("fijo_5");
    ASSERT_NE(est2, nullptr);

    auto est3 = CafeteriaFactory::obtenerEstrategia("nulo");
    ASSERT_NE(est3, nullptr);
}
