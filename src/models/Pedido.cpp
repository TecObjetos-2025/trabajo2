#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/ItemPedido.h"
#include <iostream>

// VARIABLE DE CLASE
const double Pedido::IGV = 0.18;

// Constructor para asociar un cliente a un pedido
Pedido::Pedido(int id, const Cliente *cliente)
    : id(id), cliente(cliente), estado("EN_PROCESO") {}

// Nuevo! Destructor para liberar memoria
Pedido::~Pedido()
{
    for (ItemPedido *item : items)
    {
        delete item;
    }
}

// Agregar items
void Pedido::agregarItem(const Producto *producto, int cantidad)
{
    // Añadir al final (se crea automaticamente el objeto)
    items.push_back(new ItemPedido(producto, cantidad));
}

double Pedido::calcularTotal() const
{
    double subtotal = 0.0;
    for (const auto &item : items)
    {
        subtotal += item->getSubtotal();
    }
    return subtotal * (1 + IGV);
}

// Getters
int Pedido::getId() const
{
    return id;
}

const Cliente *Pedido::getCliente() const
{
    return cliente;
}

const std::vector<ItemPedido *> &Pedido::getItems() const
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