#ifndef ITEMPEDIDO_H
#define ITEMPEDIDO_H

#include "Producto.h"

class ItemPedido
{
private:
    const Producto &producto; // Usamos una referencia al producto del menú
    int cantidad;

public:
    ItemPedido(const Producto &producto, int cantidad);

    const Producto &getProducto() const;
    int getCantidad() const;
    double getSubtotal() const; // Calcula precio * cantidad
};

#endif // ITEMPEDIDO_H