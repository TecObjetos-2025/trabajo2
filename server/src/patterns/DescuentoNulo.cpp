/**
 * @file DescuentoNulo.cpp
 * @brief Implementación de la estrategia de descuento nulo.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón Estrategia para no aplicar ningún descuento a un precio base.
 */

#include "patterns/DescuentoNulo.h"

double DescuentoNulo::aplicarDescuento(double precioBase) const
{
    return precioBase;
}

std::string DescuentoNulo::getDescripcion() const
{
    return "Sin descuento";
}
