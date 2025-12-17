/**
 * @file ItemPedido.cpp
 * @brief Implementación de la clase ItemPedido que representa un ítem en un pedido.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase maneja los ítems de un pedido, incluyendo el producto y la cantidad.
 */

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