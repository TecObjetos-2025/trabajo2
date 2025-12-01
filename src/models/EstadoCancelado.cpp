#include "../../include/models/EstadoCancelado.h"
#include "../../include/models/Pedido.h"
#include <iostream>

using namespace std;

shared_ptr<EstadoCancelado> EstadoCancelado::getInstance()
{
    static shared_ptr<EstadoCancelado> instance(new EstadoCancelado());
    return instance;
}

void EstadoCancelado::avanzar(Pedido *pedido)
{
    cout << "Pedido #" << pedido->getId() << ": El pedido esta cancelado y no puede avanzar." << endl;
}

void EstadoCancelado::cancelar(Pedido *pedido)
{
    cout << "Pedido #" << pedido->getId() << ": El pedido ya esta cancelado." << endl;
}
