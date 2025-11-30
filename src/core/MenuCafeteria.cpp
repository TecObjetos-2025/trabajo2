/**
 *  @file MenuCafeteria.cpp
 *  @brief Implementación del repositorio de productos del menú de la cafetería.
 *  @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 *  @date 2025
 *  @details Permite la gestión y consulta de productos disponibles en la cafetería.
 */

#include "core/MenuCafeteria.h"
#include <iostream>
#include <iomanip> // Formatear salida

/**
 * @brief Clase que gestiona el menú de productos disponibles en la cafetería.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @details Permite agregar, buscar y mostrar productos del menú.
 */

// Logica: Agregar nuevos productos
void MenuCafeteria::agregarProducto(const Producto &producto)
{
    /**
     * @brief Agrega un producto al menú.
     * @param producto Producto a agregar.
     */
    productos.push_back(producto);
}

// Buscar por Id (devolver un puntero)
const Producto *MenuCafeteria::getProductoPorId(int id) const
{
    /**
     * @brief Busca un producto por su ID.
     * @param id Identificador único del producto.
     * @return const Producto* Puntero al producto encontrado o nullptr si no existe.
     */
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
    /**
     * @brief Muestra el menú de la cafetería en consola.
     */
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
