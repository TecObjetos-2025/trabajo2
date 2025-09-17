#include "Pedido.h"
#include "Cliente.h"

// Constructor para asociar un cliente a un pedido
Pedido::Pedido(int id, const Cliente &cliente)
    : id(id), cliente(cliente), estado("EN_PROCESO") {}

// Agregar items
void Pedido::agregarItem(const Producto &producto, int cantidad)
{
    // Añadir al final
    items.emplace_back(producto, cantidad);
}

double Pedido::calcularTotal() const
{
    double total = 0.0;
    for (const auto &item : items)
    {
        total += item.getSubtotal();
    }
    return total;
}

// Getters
int Pedido::getId() const
{
    return id;
}

const Cliente &Pedido::getCliente() const
{
    return cliente;
}

const std::vector<ItemPedido> &Pedido::getItems() const
{
    return items;
}

std::string Pedido::getEstado() const
{
    return estado;
}

// Setters
void Pedido::setEstado(const std::string &nuevoEstado)
{
    this->estado = nuevoEstado;
}

void Pedido::marcarComoPagado()
{
    this->estado = "COMPLETADO";
}