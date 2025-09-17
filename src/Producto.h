#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <string>

class Producto
{
private:
    int id;
    std::string nombre;
    double precio;
    std::string categoria; // Ej: "Bebidas", "Postres" // Podria tambien implementarse un patron de disenio

public:
    // Constructor
    Producto(int id, const std::string &nombre, double precio, const std::string &categoria);

    // Getters
    int getId() const;
    std::string getNombre() const;
    double getPrecio() const;
    std::string getCategoria() const;

    // Setters)
    void setPrecio(double nuevoPrecio);
    void setNombre(const std::string &nuevoNombre);
};

#endif // PRODUCTO_H