#include "models/ItemPedido.h"

// Ahora se recibe puntero
ItemPedido::ItemPedido(const Producto *producto, int cantidad)
    : producto(producto), cantidad(cantidad) {}

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

// Devolver la referencia del Producto
const Producto *ItemPedido::getProducto() const { return producto; }