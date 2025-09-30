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
    std::cout << "--- Iniciando Sistema de Cafeteria (v2.0) ---" << std::endl;

    // Config
    SistemaPedidos sistema;

    // Agregar productos
    sistema.agregarProducto(new Producto(101, "Cafe Americano", 5.50, "Bebidas"));
    sistema.agregarProducto(new Producto(102, "Pan con Chicharron", 7.00, "Desayuno"));
    sistema.agregarProducto(new Producto(103, "Jugo de Papaya", 6.00, "Bebidas"));

    // Crear diferentes Personas
    Persona *cliente1 = new Cliente(1, "Carlos Juarez", "987654321");
    Persona *cocinero1 = new Cocinero(2, "Ana Guevera", "EMP-001");
    Persona *admin1 = new Administrador(3, "Juan Castro", "EMP-002");

    sistema.registrarPersona(cliente1);
    sistema.registrarPersona(cocinero1);
    sistema.registrarPersona(admin1);

    Cocinero *ptrCocinero = dynamic_cast<Cocinero *>(cocinero1);
    if (ptrCocinero)
    {
        sistema.agregarObservador(ptrCocinero);
    }

    Administrador *ptrAdmin = dynamic_cast<Administrador *>(admin1);
    if (ptrAdmin)
    {
        sistema.agregarObservador(ptrAdmin);
    }

    sistema.mostrarTodasLasPersonas();

    // -- SIMULAR PEDIDO
    std::cout << "\n --- SIMULACION --- " << std::endl;

    Cliente *ptrCliente = dynamic_cast<Cliente *>(cliente1);
    if (ptrCliente)
    {
        // Iniciar/Crear el pedido
        Pedido *nuevoPedido = new Pedido(1001, ptrCliente);

        // El cliente pide productos
        nuevoPedido->agregarItem(new Producto(101, "Cafe Americano", 5.50, "Bebidas"), 2);
        nuevoPedido->agregarItem(new Producto(102, "Pan con Chicharron", 7.00, "Desayuno"), 1);

        // Finalizar pedido
        sistema.finalizarPedido(nuevoPedido);

        delete nuevoPedido;
    }

    // Verificar Observadores
    if (ptrAdmin)
    {
        ptrAdmin->mostrarEstadisticasVentas();
    }

    std::cout << "\n--- FIN ---" << std::endl;

    return 0;
}