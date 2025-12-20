#pragma once

#include "IComandoServidor.h"

class CmdAddOrder : public IComandoServidor
{
public:
    CmdAddOrder() = default;
    ~CmdAddOrder() override = default;

    void ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema) override;
};
