/**
 * @file EstadoEnPreparacion.cpp
 * @brief Implementación del estado "En Preparación" para un pedido.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón State para manejar el estado "En Preparación" de un pedido.
 */
#include "patterns/EstadoEnPreparacion.h"
#include "patterns/EstadoListo.h"
#include "models/Pedido.h"
#include <iostream>

using namespace std;

shared_ptr<EstadoEnPreparacion> EstadoEnPreparacion::getInstance()
{
    static shared_ptr<EstadoEnPreparacion> instance(new EstadoEnPreparacion());
    return instance;
}

void EstadoEnPreparacion::avanzar(Pedido *pedido)
{
    cout << "Pedido #" << pedido->getId() << ": Finalizado. Cambiando a Listo." << endl;
    pedido->setEstado(EstadoListo::getInstance());
}

void EstadoEnPreparacion::cancelar(Pedido *pedido)
{
    cerr << "ERROR: Pedido #" << pedido->getId() << " no se puede cancelar mientras se prepara." << endl;
}