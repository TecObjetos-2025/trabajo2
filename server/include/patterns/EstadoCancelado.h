#ifndef ESTADOCANCELADO_H
#define ESTADOCANCELADO_H

#include "IEstadoPedido.h"

class EstadoCancelado : public IEstadoPedido
{
public:
    static std::shared_ptr<EstadoCancelado> getInstance();
    void avanzar(Pedido *pedido) override;
    void cancelar(Pedido *pedido) override;
    std::string getNombre() const override { return "Cancelado"; }
};

#endif
