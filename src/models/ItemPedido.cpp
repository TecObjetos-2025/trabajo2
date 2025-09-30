#include "models/ItemPedido.h"

ItemPedido::ItemPedido(const Producto &producto, int cantidad)
    : producto(producto), cantidad(cantidad) {}

double ItemPedido::getSubtotal() const
{
    return producto.getPrecio() * cantidad;
}

// Getters
int ItemPedido::getCantidad() const
{
    return cantidad;
}

// Devolver la referencia del Producto
const Producto &ItemPedido::getProducto() const { return producto; }