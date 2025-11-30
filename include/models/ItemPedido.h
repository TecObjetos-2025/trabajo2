#ifndef ITEMPEDIDO_H
#define ITEMPEDIDO_H

#include "Producto.h"
#include <memory>

/**
 * @brief Representa un ítem dentro de un pedido, asociando un producto y una cantidad.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
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