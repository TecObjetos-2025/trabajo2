#pragma once
#include <vector>
#include "models/ItemPedido.h"

// Interfaz para estrategia de descuento (Patrón Strategy)
class IEstrategiaDescuento
{
public:
    virtual ~IEstrategiaDescuento() = default;
    // Aplica el descuento sobre el precio base
    virtual double aplicarDescuento(double precioBase) const = 0;

    virtual std::string getDescripcion() const = 0;
};