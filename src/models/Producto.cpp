#include "models/Producto.h"

Producto::Producto(int id, const std::string &nombre, double precio, const std::string &categoria)
    : id(id), nombre(nombre), precio(precio), categoria(categoria) {}

// Implementacion de los Producto::getters
int Producto::getId() const { return id; }
std::string Producto::getNombre() const { return nombre; }
double Producto::getPrecio() const { return precio; }
std::string Producto::getCategoria() const { return categoria; }

// Impl. para los Producto::setters
void Producto::setPrecio(double nuevoPrecio) { precio = nuevoPrecio; }