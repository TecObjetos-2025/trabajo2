#include "CoreQtAdapter.h"
#include <QString> // Para conversión de std::string a QString

// Constructor
CoreQtAdapter::CoreQtAdapter(QObject *parent) : QObject(parent) {}

// Implementación de IObservadorCore
void CoreQtAdapter::onNuevosPedidosEnCola()
{
    // Emitir la señal correspondiente
    emit nuevosPedidosEnCola();
}

void CoreQtAdapter::onPedidoTerminado(int id_pedido)
{
    // Emitir la señal correspondiente
    emit pedidoTerminado(id_pedido);
}

void CoreQtAdapter::onError(const std::string &mensaje)
{
    // Convertir std::string a QString y emitir la señal correspondiente
    emit errorOcurrido(QString::fromStdString(mensaje));
}