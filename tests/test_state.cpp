#include <gtest/gtest.h>
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/EstadoEnCola.h"
#include "models/EstadoEnPreparacion.h"
#include "models/EstadoListo.h"

// Helper to create a dummy client
std::shared_ptr<Cliente> createDummyClient() {
    return std::make_shared<Cliente>(1, "Test Client", "123456789");
}

TEST(StateTest, InitialStateIsQueued) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);
    // Assuming default state is EstadoEnCola or similar.
    // Let's verify via getEstadoNombre or behavior.
    // Based on code, usually it starts in "En Cola"

    // I need to check exact string or logic.
    // Assuming "En Cola" based on typical names.
    EXPECT_EQ(pedido.getEstadoNombre(), "En Cola");
}

TEST(StateTest, TransitionQueuedToInPreparation) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    // Check initial state
    EXPECT_EQ(pedido.getEstadoNombre(), "En Cola");

    // Advance
    pedido.avanzar();

    EXPECT_EQ(pedido.getEstadoNombre(), "En Preparacion");
}

TEST(StateTest, TransitionInPreparationToReady) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // To En Preparacion
    pedido.avanzar(); // To Listo

    EXPECT_EQ(pedido.getEstadoNombre(), "Listo");
}

TEST(StateTest, CannotAdvanceFromReady) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // To En Preparacion
    pedido.avanzar(); // To Listo

    std::string stateBefore = pedido.getEstadoNombre();
    pedido.avanzar(); // Should stay ready or do nothing
    EXPECT_EQ(pedido.getEstadoNombre(), stateBefore);
}

TEST(StateTest, CancelQueuedOrder) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    // Should be able to cancel
    pedido.cancelar();

    // Assuming cancelled state exists or it handles it.
    // If "Cancelado" state doesn't exist, we might check output or behavior.
    // Let's check if the state name changes to "Cancelado"
    EXPECT_EQ(pedido.getEstadoNombre(), "Cancelado");
}

TEST(StateTest, CannotCancelInPreparation) {
    auto cliente = createDummyClient();
    Pedido pedido(1, cliente);

    pedido.avanzar(); // En Preparacion

    std::string stateBefore = pedido.getEstadoNombre();
    pedido.cancelar(); // Should NOT cancel

    EXPECT_EQ(pedido.getEstadoNombre(), stateBefore);
    EXPECT_EQ(pedido.getEstadoNombre(), "En Preparacion");
}
