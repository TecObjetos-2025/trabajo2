#ifndef ESTADOENPREPARACION_H
#define ESTADOENPREPARACION_H

#include "../core/EstadoPedido.h"

class EstadoEnPreparacion : public EstadoPedido {
public:
    void avanzar(Pedido* pedido) override;
    void cancelar(Pedido* pedido) override; 
    std::string getNombre() const override { return "En Preparacion"; }

    static std::shared_ptr<EstadoEnPreparacion> getInstance();
private:
    EstadoEnPreparacion() = default;
};

#endif // ESTADOENPREPARACION_H