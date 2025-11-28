#pragma once
#include "IEstrategiaDescuento.h"

class DescuentoPorcentaje : public IEstrategiaDescuento
{
    double porcentaje; // 0.10 para 10%
public:
    explicit DescuentoPorcentaje(double porcentaje_);
    double aplicarDescuento(double precioBase) const override;
};
