#include "models/ItemPedido.h"

// Ahora se recibe puntero

ItemPedido::ItemPedido(std::shared_ptr<Producto> producto, int cantidad)
    : producto(std::move(producto)), cantidad(cantidad) {}

double ItemPedido::getSubtotal() const
{
    if (producto) // Verificacion por puntero
    {
        return producto->getPrecio() * cantidad;
    }
    return 0.0;
}

// Getters

int ItemPedido::getCantidad() const
{
    return cantidad;
}

std::shared_ptr<Producto> ItemPedido::getProducto() const { return producto; }