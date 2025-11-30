#include <iostream>
#include <limits>
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
#include "core/CafeteriaFactory.h"

int main()
{
    std::cout << "--- Iniciando Sistema de Cafeteria (v2.1 con Factory) ---" << std::endl;

    SistemaPedidos sistema;

    // Agregar productos (esto sigue igual)
    auto prod1 = std::make_shared<Producto>(101, "Cafe Americano", 5.50, "Bebidas");
    auto prod2 = std::make_shared<Producto>(102, "Pan con Chicharron", 7.00, "Desayuno");
    auto prod3 = std::make_shared<Producto>(103, "Jugo de Papaya", 6.00, "Bebidas");
    sistema.agregarProducto(prod1);
    sistema.agregarProducto(prod2);
    sistema.agregarProducto(prod3);

    //  Factory para crear Personas
    auto cliente1 = CafeteriaFactory::crearCliente(1, "Carlos Juarez", "987654321");
    auto cocinero1 = CafeteriaFactory::crearCocinero(2, "Ana Guevara", "EMP-001", &sistema);
    auto admin1 = CafeteriaFactory::crearAdministrador(3, "Juan Castro", "EMP-002");

    sistema.registrarPersona(cliente1);
    sistema.registrarPersona(cocinero1);
    sistema.registrarPersona(admin1);

    sistema.registrarObservador(cocinero1);
    sistema.registrarObservador(admin1);

    Cocinero *ptrCocinero = dynamic_cast<Cocinero *>(cocinero1.get());
    Administrador *ptrAdmin = dynamic_cast<Administrador *>(admin1.get());

    if (ptrCocinero)
    {
        ptrCocinero->iniciar();
    }

    std::cout << "\n --- SIMULACION FIFO CON FACTORY --- " << std::endl;

    Cliente *ptrCliente = dynamic_cast<Cliente *>(cliente1.get());
    if (ptrCliente)
    {
        std::cout << "\n[CAJERO] Registrando 3 pedidos..." << std::endl;

        //  Factory para crear Pedidos
        auto pedido1 = CafeteriaFactory::crearPedido(1001, cliente1);
        pedido1->agregarItem(prod1, 2);
        sistema.finalizarPedido(pedido1);

        auto pedido2 = CafeteriaFactory::crearPedido(1002, cliente1);
        pedido2->agregarItem(prod2, 1);
        sistema.finalizarPedido(pedido2);

        auto pedido3 = CafeteriaFactory::crearPedido(1003, cliente1);
        pedido3->agregarItem(prod3, 1);
        sistema.finalizarPedido(pedido3);

        std::cout << "\n[MAIN] Esperando que el cocinero procese los pedidos..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(7));

        std::cout << "\n[MAIN] Verificando que la cola este vacia..." << std::endl;
        sistema.mostrarPedidosEnEspera();
        sistema.procesarSiguientePedido();
    }

    if (ptrAdmin)
    {
        ptrAdmin->mostrarEstadisticasVentas();
    }

    sistema.cerrarColaPedidos();

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

    std::cout << "\n--- DEMO STRATEGY CON FACTORY ---" << std::endl;
    // Ultima prueba con Factory
    auto pedidoDemo = CafeteriaFactory::crearPedido(2001, cliente1);
    pedidoDemo->agregarItem(prod1, 2);
    pedidoDemo->agregarItem(prod2, 1);

    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoNulo>());
    std::cout << "Total sin descuento: $" << pedidoDemo->calcularTotal() << std::endl;

    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoPorcentaje>(0.10));
    std::cout << "Total con 10% descuento: $" << pedidoDemo->calcularTotal() << std::endl;

    pedidoDemo->setEstrategiaDescuento(std::make_unique<DescuentoFijo>(5));
    std::cout << "Total con $5 descuento fijo: $" << pedidoDemo->calcularTotal() << std::endl;

    return 0;
}
