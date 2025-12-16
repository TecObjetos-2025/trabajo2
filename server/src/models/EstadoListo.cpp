/**
 * @file EstadoListo.cpp
 * @brief Implementación del estado "Listo" para un pedido.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón State para manejar el estado "Listo" de un pedido.
 */

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