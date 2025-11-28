#pragma once
#include "IEstrategiaDescuento.h"

class DescuentoFijo : public IEstrategiaDescuento
{
    double descuento;

public:
    explicit DescuentoFijo(double descuento_);
    double aplicarDescuento(double precioBase) const override;
    std::string getDescripcion() const override;
};
