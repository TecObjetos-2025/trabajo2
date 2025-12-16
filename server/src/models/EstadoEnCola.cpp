/**
 * @file EstadoEnCola.cpp
 * @brief Implementación del estado "En Cola" para un pedido.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Esta clase implementa el patrón State para manejar el estado "En Cola" de un pedido.
 */
#include "patterns/EstadoEnCola.h"
#include "patterns/EstadoEnPreparacion.h"
#include "patterns/EstadoCancelado.h"
#include "models/Pedido.h"
#include <iostream>

using namespace std;

shared_ptr<EstadoEnCola> EstadoEnCola::getInstance()
{
    static shared_ptr<EstadoEnCola> instance(new EstadoEnCola());
    return instance;
}

void EstadoEnCola::avanzar(Pedido *pedido)
{
    cout << "Pedido #" << pedido->getId() << ": Tomado por cocinero. Cambiando a Preparacion." << endl;
    pedido->setEstado(EstadoEnPreparacion::getInstance());
}

void EstadoEnCola::cancelar(Pedido *pedido)
{
    cout << "Pedido #" << pedido->getId() << ": Cancelacion exitosa desde En Cola." << endl;
    pedido->setEstado(EstadoCancelado::getInstance());
}