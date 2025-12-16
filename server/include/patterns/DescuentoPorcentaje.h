#pragma once
#include "IEstrategiaDescuento.h"

/**
 * @brief Estrategia de descuento por porcentaje para aplicar sobre el precio base.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class DescuentoPorcentaje : public IEstrategiaDescuento
{
    double porcentaje; // 0.10 para 10%
public:
    /**
     * @brief Constructor de la estrategia de descuento por porcentaje.
     * @param porcentaje_ Porcentaje de descuento a aplicar (ejemplo: 0.10 para 10%).
     */
    explicit DescuentoPorcentaje(double porcentaje_);

    /**
     * @brief Aplica el descuento sobre el precio base.
     * @param precioBase Precio original antes del descuento.
     * @return double Precio final tras aplicar el descuento.
     */
    double aplicarDescuento(double precioBase) const override;

    /**
     * @brief Devuelve la descripción textual de la estrategia de descuento.
     * @return std::string Descripción del descuento aplicado.
     */
    std::string getDescripcion() const override;
};
