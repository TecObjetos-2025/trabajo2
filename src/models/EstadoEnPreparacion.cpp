#include "../../include/models/EstadoEnPreparacion.h"
#include "../../include/models/EstadoListo.h" 
#include "../../include/models/Pedido.h"
#include <iostream>

using namespace std;

shared_ptr<EstadoEnPreparacion> EstadoEnPreparacion::getInstance() {
    static shared_ptr<EstadoEnPreparacion> instance(new EstadoEnPreparacion());
    return instance;
}

void EstadoEnPreparacion::avanzar(Pedido* pedido) {
    cout << "Pedido #" << pedido->getId() << ": Finalizado. Cambiando a Listo." << endl;
    // Transición:
    pedido->setEstado(EstadoListo::getInstance());
}

void EstadoEnPreparacion::cancelar(Pedido* pedido) {
    // Regla de negocio: Un pedido en preparación no se cancela.
    cerr << "ERROR: Pedido #" << pedido->getId() << " no se puede cancelar mientras se prepara." << endl;
}