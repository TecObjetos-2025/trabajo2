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
