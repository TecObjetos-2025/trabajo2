#include "patterns/DescuentoNulo.h"

double DescuentoNulo::aplicarDescuento(double precioBase) const
{
    return precioBase;
}

std::string DescuentoNulo::getDescripcion() const
{
    return "Sin descuento";
}
