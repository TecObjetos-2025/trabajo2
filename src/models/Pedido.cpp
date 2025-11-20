#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/ItemPedido.h"
#include <iostream>

// VARIABLE DE CLASE
const double Pedido::IGV = 0.18;

// Constructor para asociar un cliente a un pedido
Pedido::Pedido(int id, std::shared_ptr<Cliente> cliente)
    : id(id), cliente(cliente), estado("EN_PROCESO") {}

// Nuevo! Destructor para liberar memoria
Pedido::~Pedido() = default;

// Agregar items
void Pedido::agregarItem(std::shared_ptr<Producto> producto, int cantidad)
{
    items.push_back(std::make_unique<ItemPedido>(std::move(producto), cantidad));
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

std::shared_ptr<Cliente> Pedido::getCliente() const
{
    return cliente.lock();
}

const std::vector<std::unique_ptr<ItemPedido>> &Pedido::getItems() const
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