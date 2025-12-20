#pragma once

#include "IComandoServidor.h"

class CmdGetMenu : public IComandoServidor
{
public:
    CmdGetMenu() = default;
    ~CmdGetMenu() override = default;

    void ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema) override;
};
