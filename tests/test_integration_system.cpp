#include <gtest/gtest.h>
#include "core/SistemaPedidos.h"
#include "core/CafeteriaFactory.h"
#include "models/Producto.h"
#include "models/Pedido.h"
#include "models/Cocinero.h"
#include "api/IObservadorCore.h"
#include <thread>
#include <atomic>
#include <chrono>

// Mock Observer
class MockObservador : public IObservadorCore {
public:
    std::atomic<int> pedidosNuevos{0};
    std::atomic<int> pedidosTerminados{0};
    std::atomic<int> ultimoPedidoTerminadoId{-1};

    void onNuevosPedidosEnCola() override {
        pedidosNuevos++;
    }

    void onPedidoTerminado(int idPedido) override {
        pedidosTerminados++;
        ultimoPedidoTerminadoId = idPedido;
    }

    void onError(const std::string &mensaje) override {
        // Mock implementation, maybe log or ignore
    }
};

TEST(IntegrationTest, FullSystemFlow) {
    SistemaPedidos sistema;

    // 1. Setup Data
    auto prod1 = std::make_shared<Producto>(1, "Cafe", 5.0, "Bebidas");
    sistema.agregarProducto(prod1);

    auto cliente = CafeteriaFactory::crearCliente(10, "Tester", "123");

    auto observador = std::make_shared<MockObservador>();
    sistema.registrarObservador(observador);

    // 2. Create Order
    auto pedido = CafeteriaFactory::crearPedido(999, cliente);
    pedido->agregarItem(prod1, 1);

    // 3. Finalize Order (This puts it in Queue and Notifies)
    sistema.finalizarPedido(pedido);

    EXPECT_EQ(observador->pedidosNuevos, 1);

    // Verify Queue
    // SistemaPedidos has `mostrarPedidosEnEspera` which prints.
    // It has `getPedidosEnCola` returning DTOs.
    auto enCola = sistema.getPedidosEnCola();
    ASSERT_EQ(enCola.size(), 1);
    EXPECT_EQ(enCola[0].id_pedido, 999);

    // 4. Consumer (Cook) Logic simulation
    // We can simulate what `Cocinero` does or use `SistemaPedidos::procesarSiguientePedidoInterno`

    // Let's create a thread to act as Cook
    std::atomic<bool> done(false);
    std::thread cookThread([&sistema, &done, &observador]() {
        // Wait for order
        auto pedidoProcesado = sistema.procesarSiguientePedidoInterno();
        if (pedidoProcesado) {
            // Simulate cooking time (fast)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // Notify completion
            sistema.notificarPedidoTerminado(pedidoProcesado);
            done = true;
        }
    });

    cookThread.join();

    EXPECT_TRUE(done);
    EXPECT_EQ(observador->pedidosTerminados, 1);
    EXPECT_EQ(observador->ultimoPedidoTerminadoId, 999);
}
