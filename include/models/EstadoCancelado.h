#ifndef ESTADOCANCELADO_H
#define ESTADOCANCELADO_H

#include "../../include/core/EstadoPedido.h"

class EstadoCancelado : public EstadoPedido {
public:
    static std::shared_ptr<EstadoCancelado> getInstance();
    void avanzar(Pedido* pedido) override;
    void cancelar(Pedido* pedido) override;
    std::string getNombre() const override { return "Cancelado"; }
};

#endif
