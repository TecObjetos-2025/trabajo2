/**
 * @file DescuentoPorcentaje.cpp
 * @brief Implementación de la estrategia de descuento por porcentaje.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón Estrategia para aplicar un descuento porcentual a un precio base.
 */

#include "patterns/DescuentoPorcentaje.h"

DescuentoPorcentaje::DescuentoPorcentaje(double porcentaje_) : porcentaje(porcentaje_) {}

double DescuentoPorcentaje::aplicarDescuento(double precioBase) const
{
    return precioBase * (1.0 - porcentaje);
}
std::string DescuentoPorcentaje::getDescripcion() const
{
    return "Descuento del " + std::to_string(porcentaje * 100) + "%";
}
