#ifndef MENUCAFETERIA_H
#define MENUCAFETERIA_H

#include <vector>
#include "models/Producto.h"

class MenuCafeteria
{
private:
    std::vector<Producto> productos;

public:
    void agregarProducto(const Producto &producto);
    const Producto *getProductoPorId(int id) const; // Devuelve un puntero al producto
    void mostrarMenu() const;
};

#endif // MENUCAFETERIA_H