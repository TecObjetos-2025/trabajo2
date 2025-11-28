#include <iostream>
#include <limits> // Para limpiar cin
#include <thread>
#include "core/SistemaPedidos.h"
#include "models/Persona.h"
#include "models/Cliente.h"
#include "models/Empleado.h"
#include "models/Cocinero.h"
#include "models/Administrador.h"
#include "models/Producto.h"
#include "models/Pedido.h"
#include "patterns/DescuentoNulo.h"
#include "patterns/DescuentoPorcentaje.h"
#include "patterns/DescuentoFijo.h"

int main()
{
    std::cout << "--- Iniciando Sistema de Cafeteria (v2.0 + cambios practica 5) ---" << std::endl;

    // Config
    SistemaPedidos sistema;

    // Agregar productos
    auto prod1 = std::make_shared<Producto>(101, "Cafe Americano", 5.50, "Bebidas");
    auto prod2 = std::make_shared<Producto>(102, "Pan con Chicharron", 7.00, "Desayuno");
    auto prod3 = std::make_shared<Producto>(103, "Jugo de Papaya", 6.00, "Bebidas");
    sistema.agregarProducto(prod1);
    sistema.agregarProducto(prod2);
    sistema.agregarProducto(prod3);

    // Crear diferentes Personas
    auto cliente1 = std::make_shared<Cliente>(1, "Carlos Juarez", "987654321");
    auto cocinero1 = std::make_shared<Cocinero>(2, "Ana Guevara", "EMP-001", &sistema);
    auto admin1 = std::make_shared<Administrador>(3, "Juan Castro", "EMP-002");

    sistema.registrarPersona(cliente1);
    sistema.registrarPersona(cocinero1);
    sistema.registrarPersona(admin1);

    // Configurar Observadores

    sistema.registrarObservador(cocinero1);
    sistema.registrarObservador(admin1);

    Cocinero *ptrCocinero = dynamic_cast<Cocinero *>(cocinero1.get());
    Administrador *ptrAdmin = dynamic_cast<Administrador *>(admin1.get());

    // Iniciar hilo del cocinero
    if (ptrCocinero)
    {
        ptrCocinero->iniciar();
    }

    // sistema.mostrarTodasLasPersonas();

    // -- SIMULACION PRACTICA 5 --
    std::cout << "\n --- SIMULACION FIFO (PRACTICA 5) --- " << std::endl;

    Cliente *ptrCliente = dynamic_cast<Cliente *>(cliente1.get());
    if (ptrCliente)
    {
        // 1. Cajero crear y encola pedidos
        std::cout << "\n[CAJERO] Registrando 3 pedidos..." << std::endl;

        // Pedido 1 (P1)
        auto pedido1 = std::make_shared<Pedido>(1001, cliente1);
        pedido1->agregarItem(prod1, 2);
        sistema.finalizarPedido(pedido1);

        auto pedido2 = std::make_shared<Pedido>(1002, cliente1);
        pedido2->agregarItem(prod2, 1);
        sistema.finalizarPedido(pedido2);

        auto pedido3 = std::make_shared<Pedido>(1003, cliente1);
        pedido3->agregarItem(prod3, 1);
        sistema.finalizarPedido(pedido3);

        // Esperar a que el cocinero procese los pedidos
        std::cout << "\n[MAIN] Esperando que el cocinero procese los pedidos..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(7)); // Ajusta el tiempo según la cantidad de pedidos

        // Verificar que la cola este vacia
        std::cout << "\n[MAIN] Verificando que la cola este vacia..." << std::endl;
        sistema.mostrarPedidosEnEspera();  // Debe mostrar [COLA VACIA]
        sistema.procesarSiguientePedido(); // Debe mostrar error "No hay pedidos"
    }

    // Verificar Observadores (Stats de Admin)
    if (ptrAdmin)
    {
        ptrAdmin->mostrarEstadisticasVentas();
    }

    sistema.cerrarColaPedidos(); // Cerrar la cola para terminar el hilo del cocinero

    if (ptrCocinero)
    {
        try
        {
            ptrCocinero->detener();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error al detener el cocinero: " << e.what() << std::endl;
        }
    }

    // --- DEMO STRATEGY
    std::cout << "\n--- DEMO STRATEGY: Descuentos dinámicos en Pedido ---" << std::endl;
    auto pedidoDemo = std::make_shared<Pedido>(2001, cliente1);
    pedidoDemo->agregarItem(prod1, 2); // 2x Café Americano
    pedidoDemo->agregarItem(prod2, 1); // 1x Pan con Chicharrón

    // Sin descuento (DescuentoNulo)
    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoNulo>());
    std::cout << "Total sin descuento: $" << pedidoDemo->calcularTotal() << std::endl;

    // Descuento porcentual (10%)
    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoPorcentaje>(0.10));
    std::cout << "Total con 10% descuento: $" << pedidoDemo->calcularTotal() << std::endl;

    // Descuento fijo ($5.00)
    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoFijo>(5.00));
    std::cout << "Total con descuento fijo de $5.00: $" << pedidoDemo->calcularTotal() << std::endl;

    return 0;
}