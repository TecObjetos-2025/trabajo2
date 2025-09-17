#include <iostream>
#include <limits> // Para limpiar cin
#include "SistemaPedidos.h"
#include "Cocina.h"

// Funciones Auxiliares
void mostrarMenuPrincipal();
void registrarNuevoCliente(SistemaPedidos &sistema);
void crearNuevoPedido(SistemaPedidos &sistema);

int main()
{
    SistemaPedidos sistema;
    Cocina cocina;

    sistema.inicializarMenu();
    sistema.agregarObservador(&cocina);

    std::cout << "Bienvenido al Sistema de Gestion de la Cafeteria!" << std::endl;

    int opcion = 0;
    while (opcion != 4)
    {
        mostrarMenuPrincipal();
        std::cin >> opcion;

        // Validad la entrada
        if (std::cin.fail())
        {
            std::cout << "Ops! Pro favor, ingresa un numero valido!" << std::endl;
            std::cin.clear();
            // Para ignorar la mala entrada (https://www.geeksforgeeks.org/cpp/cin-ignore-function-in-cpp/)
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; // Regresar
        }

        switch (opcion)
        {
        case 1:
            sistema.mostrarMenu();
            break;
        case 2:
            registrarNuevoCliente(sistema);
            break;
        case 3:
            crearNuevoPedido(sistema);
            break;
        case 4:
            std::cout << "Saliendo del sistema. Bye!!" << std::endl;
            break;
        default:
            std::cout << "Ops! La opcion no es valida. Intenta de nuevo!" << std::endl;
            break;
        }
        std::cout << std::endl;
    }

    return 0;
}

// Funciones auxiliares
void mostrarMenuPrincipal()
{
    std::cout << "----------- MENU PRINCIPAL -----------" << std::endl;
    std::cout << "1. Mostrar Menu de la Cafeteria" << std::endl;
    std::cout << "2. Registrar Nuevo Cliente" << std::endl;
    std::cout << "3. Crear Nuevo Pedido" << std::endl;
    std::cout << "4. Salir" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

void registrarNuevoCliente(SistemaPedidos &sistema)
{
    std::string nombre, telefono;
    std::cout << "Ingrese el nombre del cliente: ";
    std::cin.ignore(); // Limpiar el buffer
    std::getline(std::cin, nombre);
    std::cout << "Ingrese el telefono del cliente: ";
    std::getline(std::cin, telefono);
    sistema.registrarCliente(nombre, telefono);
}

void crearNuevoPedido(SistemaPedidos &sistema)
{
    int idCliente;
    std::cout << "Ingrese el ID del cliente para el pedido: ";
    std::cin >> idCliente;

    Cliente *cliente = sistema.buscarClientePorId(idCliente);
    if (cliente == nullptr)
    {
        std::cout << "Cliente con ID " << idCliente << " no encontrado." << std::endl;
        return;
    }

    Pedido &nuevoPedido = sistema.crearPedido(*cliente);
    std::cout << "Pedido #" << nuevoPedido.getId() << " creado para el cliente " << cliente->getNombre() << "." << std::endl;

    int idProducto = 0;
    while (idProducto != -1)
    {
        sistema.mostrarMenu();
        std::cout << "Ingrese el ID del producto para agregar (o -1 para finalizar): ";
        std::cin >> idProducto;

        if (idProducto != -1)
        {
            int cantidad;
            std::cout << "Ingrese la cantidad: ";
            std::cin >> cantidad;
            if (cantidad > 0)
            {
                sistema.agregarProductoAPedido(nuevoPedido, idProducto, cantidad);
            }
            else
            {
                std::cout << "La cantidad debe ser mayor a cero." << std::endl;
            }
        }
    }

    // Al finalizar el pedido, se notifica a los observadores (la cocina)
    std::cout << "Finalizando pedido..." << std::endl;
    nuevoPedido.setEstado("COMPLETADO");
    sistema.notificarObservadores(nuevoPedido);
}