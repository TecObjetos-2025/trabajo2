#include "SistemaPedidos.h"
#include <iostream>

// Constructor
SistemaPedidos::SistemaPedidos() {}

// Mejora de delegacion al mostrar el menu
void SistemaPedidos::mostrarMenu() const
{
    this->menu.mostrarMenu();
}

void SistemaPedidos::inicializarMenu()
{
    menu.agregarProducto(Producto(1, "Cafe Americano", 2.50, "Bebidas"));
    menu.agregarProducto(Producto(2, "Cafe Expresso", 3.00, "Bebidas"));
    menu.agregarProducto(Producto(3, "Pan con Chicharron", 3.20, "Desayuno"));
    menu.agregarProducto(Producto(4, "Cheesecake", 4.50, "Postres"));
    menu.agregarProducto(Producto(5, "Jugo de Naranja", 2.00, "Bebidas"));
}

void SistemaPedidos::registrarCliente(const std::string &nombre, const std::string &telefono)
{
    clientes.emplace_back(proximoIdCliente++, nombre, telefono);
    std::cout << "Cliente '" << nombre << "' registrado con exito con el ID: " << (proximoIdCliente - 1) << std::endl;
}

Cliente *SistemaPedidos::buscarClientePorId(int id)
{
    for (auto &cliente : clientes)
    {
        if (cliente.getId() == id)
        {
            return &cliente;
        }
    }
    return nullptr;
}

// Crear un nuevo pedido
Pedido &SistemaPedidos::crearPedido(Cliente &cliente)
{
    pedidos.emplace_back(proximoIdPedido++, cliente);
    return pedidos.back();
}

// Agregar un producto a un pedido existente.
void SistemaPedidos::agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad)
{
    const Producto *producto = menu.getProductoPorId(idProducto);
    if (producto != nullptr)
    {
        pedido.agregarItem(*producto, cantidad);
        std::cout << "Se agregaron " << cantidad << "x " << producto->getNombre() << " al pedido." << std::endl;
    }
    else
    {
        std::cout << "Hubo un error: Producto con ID " << idProducto << " no encontrado en el menu." << std::endl;
    }
}
void SistemaPedidos::finalizarPedido(Pedido &pedido)
{
    pedido.marcarComoPagado();
    std::cout << "Pedido #" << pedido.getId()
              << " finalizado y marcado como PAGADO." << std::endl;
}

// Metodos del Observador
// Agrega a la cocina, por ejemplo, como nuevo observador
void SistemaPedidos::agregarObservador(Observador *obs)
{
    observadores.push_back(obs);
}

// Notificar a todos los observadores en la lista
void SistemaPedidos::notificarObservadores(const Pedido &pedido)
{
    std::cout << "\n[SISTEMA] Notificando a " << observadores.size() << " observador(es)..." << std::endl;
    for (auto *obs : observadores)
    {
        obs->actualizar(pedido);
    }
}