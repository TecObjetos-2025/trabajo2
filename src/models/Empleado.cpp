#include "models/Empleado.h"
#include <iostream>

Empleado::Empleado(int id, const std::string &nombre, const std::string &codigoEmpleado)
    : Persona(id, nombre), codigoEmpleado(codigoEmpleado) {}

void Empleado::mostrarInfo() const
{
    std::cout << "--- Ficha de Empleado ---" << std::endl;
    std::cout << "ID: " << id << ", Nombre: " << nombre
              << ", Codigo: " << codigoEmpleado << std::endl;
}