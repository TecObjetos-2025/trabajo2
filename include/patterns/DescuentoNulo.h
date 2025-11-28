#pragma once
#include "IEstrategiaDescuento.h"

class DescuentoNulo : public IEstrategiaDescuento
{
public:
    double aplicarDescuento(double precioBase) const override;
};
