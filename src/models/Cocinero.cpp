#include "models/Cocinero.h"
#include "models/Pedido.h"
#include <iostream>

Cocinero::Cocinero(int id, const std::string &nombre, const std::string &codigoEmpleado)
    : Empleado(id, nombre, codigoEmpleado) {}

void Cocinero::mostrarInfo() const
{
    std::cout << "--- Ficha de Cocinero ---" << std::endl;
    std::cout << "ID: " << id << ", Nombre: " << nombre
              << ", Puesto: Cocinero, Codigo: " << codigoEmpleado << std::endl;
}

void Cocinero::actualizar(const Pedido *pedido)
{
    std::cout << "\n================ AVISO PARA COCINA ================" << std::endl;
    std::cout << "¡Nuevo pedido #" << pedido->getId() << " para preparar!" << std::endl;
    std::cout << "===================================================" << std::endl;
}