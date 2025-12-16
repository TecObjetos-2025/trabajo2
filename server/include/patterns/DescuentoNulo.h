#pragma once
#include "IEstrategiaDescuento.h"

/**
 * @brief Estrategia de descuento nulo (sin descuento) para aplicar sobre el precio base.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class DescuentoNulo : public IEstrategiaDescuento
{
public:
    double aplicarDescuento(double precioBase) const override;
    std::string getDescripcion() const override;
};
