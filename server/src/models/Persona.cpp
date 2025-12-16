#include "models/Persona.h"

Persona::Persona(int id, const std::string &nombre)
    : id(id), nombre(nombre) {}

std::string Persona::getNombre() const
{
    return nombre;
}

int Persona::getId() const
{
    return id;
}