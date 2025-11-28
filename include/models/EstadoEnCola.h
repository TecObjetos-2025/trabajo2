
#ifndef ESTADOENCOLA_H
#define ESTADOENCOLA_H

#include "core/EstadoPedido.h"

/**
 * @brief Estado concreto "En Cola" para el patrón State de Pedido.
 * Representa el estado inicial de un pedido, donde aún puede ser cancelado.
 */
class EstadoEnCola : public EstadoPedido
{
public:
    /**
     * @brief Avanza el estado del pedido desde "En Cola" a "En Preparación".
     */
    void avanzar(Pedido *pedido) override;

    /**
     * @brief Permite cancelar el pedido mientras está en cola.
     */
    void cancelar(Pedido *pedido) override;

    /**
     * @brief Obtiene el nombre descriptivo del estado.
     */
    std::string getNombre() const override { return "En Cola"; }

    /**
     * @brief Acceso a la instancia única de este estado (Singleton).
     */
    static std::shared_ptr<EstadoEnCola> getInstance();

private:
    EstadoEnCola() = default;
};

#endif // ESTADOENCOLA_H