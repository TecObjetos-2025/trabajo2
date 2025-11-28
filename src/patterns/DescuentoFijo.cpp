#include "patterns/DescuentoFijo.h"

DescuentoFijo::DescuentoFijo(double descuento_) : descuento(descuento_) {}

double DescuentoFijo::aplicarDescuento(double precioBase) const
{
    return precioBase - descuento;
}
std::string DescuentoFijo::getDescripcion() const
{
    return "Descuento fijo de $" + std::to_string(descuento);
}
