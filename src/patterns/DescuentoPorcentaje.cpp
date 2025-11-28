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
