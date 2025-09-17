#include "Cliente.h"

// Constructor
Cliente::Cliente(int id, const std::string &nombre, const std::string &telefono)
    : id(id), nombre(nombre), telefono(telefono) {}

// Getters
int Cliente::getId() const
{
    return id;
}
std::string Cliente::getNombre() const { return nombre; }