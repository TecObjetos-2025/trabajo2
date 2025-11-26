#ifndef ITEMPEDIDO_H
#define ITEMPEDIDO_H

#include "Producto.h"
#include <memory>

class ItemPedido
{
private:
    std::shared_ptr<Producto> producto;
    int cantidad;

public:
    ItemPedido(std::shared_ptr<Producto> producto, int cantidad);

    std::shared_ptr<Producto> getProducto() const;
    int getCantidad() const;
    double getSubtotal() const;
};

#endif // ITEMPEDIDO_H