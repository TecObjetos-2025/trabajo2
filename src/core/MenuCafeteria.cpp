#include "core/MenuCafeteria.h"
#include <iostream>
#include <iomanip> // Formatear salida

// Logica: Agregar nuevos productos
void MenuCafeteria::agregarProducto(const Producto &producto)
{
    productos.push_back(producto);
}

// Buscar por Id (devolver un puntero)
const Producto *MenuCafeteria::getProductoPorId(int id) const
{
    for (const auto &producto : productos)
    {
        if (producto.getId() == id)
        {
            return &producto;
        }
    }
    return nullptr; // No se encontro
}

// Mostrar el Menu (como un toString() en Java)
void MenuCafeteria::mostrarMenu() const
{
    std::cout << "----------- MENU DE LA CAFETERIA -----------" << std::endl;
    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(25) << "Nombre"
              << std::setw(15) << "Categoria"
              << std::right << std::setw(10) << "Precio" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    // Fijar la precision de los precios
    std::cout << std::fixed << std::setprecision(2);

    for (const auto &producto : productos)
    {
        std::cout << std::left << std::setw(5) << producto.getId()
                  << std::setw(25) << producto.getNombre()
                  << std::setw(15) << producto.getCategoria()
                  << std::right << std::setw(10) << producto.getPrecio() << std::endl;
    }
    std::cout << "---------------------------------------------------" << std::endl;
}
