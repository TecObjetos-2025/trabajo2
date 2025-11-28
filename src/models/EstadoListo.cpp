#include "../../include/models/EstadoListo.h"
#include "../../include/models/Pedido.h"
#include <iostream>

using namespace std;

shared_ptr<EstadoListo> EstadoListo::getInstance()
{
    static shared_ptr<EstadoListo> instance(new EstadoListo());
    return instance;
}

void EstadoListo::avanzar(Pedido *pedido)
{
    cout << "El Pedido #" << pedido->getId() << " ya esta Listo. Esperando ser entregado." << endl;
}

void EstadoListo::cancelar(Pedido *pedido)
{
    cerr << "ERROR: El Pedido #" << pedido->getId() << " ya está listo. No se puede cancelar." << endl;
}