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

/**
 * @file test_integration_system.cpp
 * @brief Pruebas de integración para el sistema de gestión de pedidos.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Este archivo contiene pruebas de integración que verifican el flujo completo
 * desde la creación de un pedido hasta su procesamiento por un cocinero, incluyendo
 * la notificación a los observadores.
 */

// Mock Observer
class MockObservador : public IObservadorCore
{
public:
    std::atomic<int> pedidosNuevos{0};
    std::atomic<int> pedidosTerminados{0};
    std::atomic<int> ultimoPedidoTerminadoId{-1};

    void onNuevosPedidosEnCola() override
    {
        pedidosNuevos++;
    }

    void onPedidoTerminado(int idPedido) override
    {
        pedidosTerminados++;
        ultimoPedidoTerminadoId = idPedido;
    }

    void onError(const std::string &mensaje) override
    {
        // Mock de implementación, tal vez registrar o ignorar
    }
};

TEST(IntegrationTest, FullSystemFlow)
{
    SistemaPedidos sistema;

    // 1. Setear Datos
    auto prod1 = std::make_shared<Producto>(1, "Cafe", 5.0, "Bebidas");
    sistema.agregarProducto(prod1);

    auto cliente = CafeteriaFactory::crearCliente(10, "Tester", "123");

    auto observador = std::make_shared<MockObservador>();
    sistema.registrarObservador(observador);

    // 2. Crear Pedido
    auto pedido = CafeteriaFactory::crearPedido(999, cliente);
    pedido->agregarItem(prod1, 1);

    // 3. Finalizar Pedido (Esto lo pone en Cola y Notifica)
    sistema.finalizarPedido(pedido);

    EXPECT_EQ(observador->pedidosNuevos, 1);

    auto activos = sistema.getPedidosActivos(); // Antes getPedidosEnCola
    ASSERT_EQ(activos.size(), 1);
    EXPECT_EQ(activos[0].id_pedido, 999);
    EXPECT_EQ(activos[0].estado, "En Cola");

    // 4. Consumidor (Cocinero) Lógica simulación

    // Simular que un cocinero procesa el pedido.
    std::atomic<bool> done(false);
    std::thread cookThread([&sistema, &done]()
                           {
        // Esperar hasta que haya un pedido en cola
        auto pedidoProcesado = sistema.procesarSiguientePedidoInterno();
        if (pedidoProcesado) {
            pedidoProcesado->avanzar(); // Estado: En Cola -> En Preparacion
            pedidoProcesado->avanzar(); // Estado: En Preparacion -> Listo
            // Notificar finalización
            sistema.notificarPedidoTerminado(pedidoProcesado);
            done = true;
        } });

    cookThread.join();
    EXPECT_TRUE(done);

    auto activosFinal = sistema.getPedidosActivos();
    ASSERT_EQ(activosFinal.size(), 1);
    EXPECT_EQ(activosFinal[0].id_pedido, 999);
    EXPECT_EQ(activosFinal[0].estado, "Listo");
}
