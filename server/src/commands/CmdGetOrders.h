#pragma once

#include <memory>
#include <QTcpSocket>
#include <QJsonObject>

#include "commands/IComandoServidor.h"

class SistemaPedidos;

class CmdGetOrders : public IComandoServidor
{
public:
    CmdGetOrders() = default;
    virtual ~CmdGetOrders() = default;

    void ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema) override;
};
