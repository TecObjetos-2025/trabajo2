#include <iostream>
#include <limits> // Para limpiar cin
#include "core/SistemaPedidos.h"
#include "models/Persona.h"
#include "models/Cliente.h"
#include "models/Empleado.h"
#include "models/Cocinero.h"
#include "models/Administrador.h"
#include "models/Producto.h"
#include "models/Pedido.h"

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

        // 2. Mostrar cola de pedidos en espera
        // (Esto debe mostrar las direcciones de memoria de P1, P2, P3 en orden)
        sistema.mostrarPedidosEnEspera();

        // 3. Cocinero procesa los pedidos en orden FIFO
        std::cout << "\n[MAIN] Dando orden al cocinero de trabajar con la FIFO..." << std::endl;

        ptrCocinero->cocinarSiguientePedido(); // Procesa Pedido 1
        ptrCocinero->cocinarSiguientePedido(); // Procesa Pedido 2

        // Verificar el estado de la cola (Solo queda el Pedido 3)
        std::cout << "\n[CAJERO] (Verificando estado de la cola antes del ultimo pedido...)" << std::endl;
        sistema.mostrarPedidosEnEspera();

        ptrCocinero->cocinarSiguientePedido(); // Procesa Pedido 3

        // 4. Verificar que la cola este vacia
        std::cout << "\n[MAIN] Verificando que la cola este vacia..." << std::endl;
        sistema.mostrarPedidosEnEspera();  // Debe mostrar [COLA VACIA]
        sistema.procesarSiguientePedido(); // Debe mostrar error "No hay pedidos"
    }

    // Verificar Observadores (Stats de Admin)
    if (ptrAdmin)
    {
        ptrAdmin->mostrarEstadisticasVentas();
    }

    // --- PRUEBAS DE LA API/FACHADA ---
    std::cout << "\n--- PRUEBAS DE LA API/FACHADA ---" << std::endl;

    // Mostrar menú limpio (DTO)
    std::vector<InfoProducto> menuDTO = sistema.getMenu();
    std::cout << "\n[API] Menu disponible:" << std::endl;
    for (const auto &prod : menuDTO)
    {
        std::cout << "  - " << prod.id << ": " << prod.nombre << " ($" << prod.precio << ")" << std::endl;
    }

    // Mostrar descuentos disponibles
    std::vector<InfoDescuento> descuentosDTO = sistema.getDescuentosDisponibles();
    std::cout << "\n[API] Descuentos disponibles:" << std::endl;
    for (const auto &desc : descuentosDTO)
    {
        std::cout << "  - " << desc.id_descuento << ": " << desc.descripcion << std::endl;
    }
    if (descuentosDTO.empty())
        std::cout << "  (Ninguno)" << std::endl;

    // Mostrar pedidos en cola (stub/documentado)
    std::vector<InfoPedido> pedidosDTO = sistema.getPedidosEnCola();
    std::cout << "\n[API] Pedidos en cola:" << std::endl;
    for (const auto &pedido : pedidosDTO)
    {
        std::cout << "  - Pedido #" << pedido.id_pedido << " de " << pedido.cliente << " (" << pedido.estado << ")" << std::endl;
        for (const auto &item : pedido.items)
        {
            std::cout << "      * " << item.cantidad << "x " << item.nombreProducto << " ($" << item.precioUnitario << ")" << std::endl;
        }
        std::cout << "    Total: $" << pedido.total_final << std::endl;
    }
    if (pedidosDTO.empty())
        std::cout << "  (Ninguno o pendiente de refactorización de Cola)" << std::endl;

    std::cout << "\n--- FIN ---" << std::endl;

    return 0;
}