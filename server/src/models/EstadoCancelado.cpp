/**
 * @file EstadoCancelado.cpp
 * @brief Implementación del estado "Cancelado" para un pedido.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón State para manejar el estado "Cancelado" de un pedido.
 */
#include "patterns/EstadoCancelado.h"
#include "models/Pedido.h"
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
