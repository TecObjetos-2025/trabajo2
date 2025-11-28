
#ifndef ESTADOENPREPARACION_H
#define ESTADOENPREPARACION_H

#include "core/EstadoPedido.h"

/**
 * @brief Estado concreto "En Preparación" para el patrón State de Pedido.
 * Representa el estado donde el pedido está siendo preparado y no puede ser cancelado.
 */
class EstadoEnPreparacion : public EstadoPedido
{
public:
    /**
     * @brief Avanza el estado del pedido desde "En Preparación" a "Listo".
     */
    void avanzar(Pedido *pedido) override;

    /**
     * @brief No permite cancelar el pedido en este estado.
     */
    void cancelar(Pedido *pedido) override;

    /**
     * @brief Obtiene el nombre descriptivo del estado.
     */
    std::string getNombre() const override { return "En Preparacion"; }

    /**
     * @brief Acceso a la instancia única de este estado (Singleton).
     */
    static std::shared_ptr<EstadoEnPreparacion> getInstance();

private:
    EstadoEnPreparacion() = default;
};

#endif // ESTADOENPREPARACION_H