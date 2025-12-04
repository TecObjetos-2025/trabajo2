/**
 * @file Cliente.cpp
 * @brief Implementación de la clase Cliente que hereda de Persona.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase representa a un cliente de la cafetería con información adicional como el teléfono.
 */

#include "models/Cliente.h"
#include <iostream>

// Constructor
Cliente::Cliente(int id, const std::string &nombre, const std::string &telefono)
    : Persona(id, nombre), telefono(telefono) {}

std::string Cliente::getTelefono() const
{
    return telefono;
}

void Cliente::mostrarInfo() const
{
    std::cout << "--- Ficha de Cliente ---" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Nombre: " << nombre << std::endl;
    std::cout << "Telefono: " << telefono << std::endl;
}
