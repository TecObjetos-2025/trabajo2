#pragma once
#include "IEstrategiaDescuento.h"

/**
 * @brief Estrategia de descuento fijo para aplicar sobre el precio base.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class DescuentoFijo : public IEstrategiaDescuento
{
    double descuento;

public:
    explicit DescuentoFijo(double descuento_);
    double aplicarDescuento(double precioBase) const override;
    std::string getDescripcion() const override;
};
