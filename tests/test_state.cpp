#include <gtest/gtest.h>
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/EstadoEnCola.h"
#include "models/EstadoEnPreparacion.h"
#include "models/EstadoListo.h"

/**
 * @file test_state.cpp
 * @brief Pruebas unitarias para el patrón State en la clase Pedido utilizando Google Test.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Este archivo contiene pruebas para verificar las transiciones de estado
 * en la clase Pedido conforme al patrón State.
 */

// Ayuda para crear un cliente dummy
std::shared_ptr<Cliente> createDummyClient()
{
    return std::make_shared<Cliente>(1, "Test Client", "123456789");
}

TEST(StateTest, InitialStateIsQueued)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    // Chequear estado inicial
    EXPECT_EQ(pedido.getEstadoNombre(), "En Cola");
}

TEST(StateTest, TransitionQueuedToInPreparation)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    // Chequear estado inicial
    EXPECT_EQ(pedido.getEstadoNombre(), "En Cola");

    pedido.avanzar();

    EXPECT_EQ(pedido.getEstadoNombre(), "En Preparacion");
}

TEST(StateTest, TransitionInPreparationToReady)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // A En Preparacion
    pedido.avanzar(); // A Listo

    EXPECT_EQ(pedido.getEstadoNombre(), "Listo");
}

TEST(StateTest, CannotAdvanceFromReady)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // A En Preparacion
    pedido.avanzar(); // A Listo

    std::string stateBefore = pedido.getEstadoNombre();
    pedido.avanzar(); // Debería no cambiar
    EXPECT_EQ(pedido.getEstadoNombre(), stateBefore);
}

TEST(StateTest, CancelQueuedOrder)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    // Debería poder cancelar
    pedido.cancelar();

    // Se asume estado "Cancelado"
    EXPECT_EQ(pedido.getEstadoNombre(), "Cancelado");
}

TEST(StateTest, CannotCancelInPreparation)
{
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // En Preparacion

    std::string stateBefore = pedido.getEstadoNombre();
    pedido.cancelar(); // Debería no cambiar

    EXPECT_EQ(pedido.getEstadoNombre(), stateBefore);
    EXPECT_EQ(pedido.getEstadoNombre(), "En Preparacion");
}
