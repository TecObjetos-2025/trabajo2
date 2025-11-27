#include "../../include/models/EstadoEnCola.h"
#include "../../include/models/EstadoEnPreparacion.h" 
#include "../../include/models/Pedido.h" 
#include <iostream>

using namespace std;

shared_ptr<EstadoEnCola> EstadoEnCola::getInstance() {
    static shared_ptr<EstadoEnCola> instance(new EstadoEnCola());
    return instance;
}

void EstadoEnCola::avanzar(Pedido* pedido) {
    cout << "Pedido #" << pedido->getId() << ": Tomado por cocinero. Cambiando a Preparacion." << endl;
    // Transición: Usa setEstado del Pedido para cambiar el puntero
    pedido->setEstado(EstadoEnPreparacion::getInstance());
}

void EstadoEnCola::cancelar(Pedido* pedido) {
    cout << "Pedido #" << pedido->getId() << ": Cancelacion exitosa desde En Cola." << endl;
    // Opcional: Podrías transicionar a un EstadoCancelado si lo creas.
}