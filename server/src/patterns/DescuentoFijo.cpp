/**
 * @file DescuentoFijo.cpp
 * @brief Implementación de la estrategia de descuento fijo.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón Estrategia para aplicar un descuento fijo a un precio base.
 */

#include "patterns/DescuentoFijo.h"

DescuentoFijo::DescuentoFijo(double descuento_) : descuento(descuento_) {}

#include <algorithm>

double DescuentoFijo::aplicarDescuento(double precioBase) const
{
    return std::max(0.0, precioBase - descuento);
}
std::string DescuentoFijo::getDescripcion() const
{
    return "Descuento fijo de $" + std::to_string(descuento);
}
