#include "NetworkServer.h"

#include "CmdGetMenu.h"
#include "CmdAddOrder.h"
#include "api/Protocolo.h"

#include <QHostAddress>
#include <iostream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include "core/SistemaPedidos.h"

NetworkServer::NetworkServer(SistemaPedidos *sistemaPtr, QObject *parent)
    : QTcpServer(parent), sistema(sistemaPtr)
{
    // Iniciar escucha en todas las interfaces en el puerto por defecto
    if (!this->listen(QHostAddress::Any, Protocolo::DEFAULT_PORT))
    {
        std::cerr << "NetworkServer: No se pudo bindear al puerto " << Protocolo::DEFAULT_PORT << std::endl;
    }
    else
    {
        std::cout << "NetworkServer: escuchando en puerto " << Protocolo::DEFAULT_PORT << std::endl;
    }

    // Registrar comandos disponibles
    comandos.insert({Protocolo::CMD_GET_MENU, std::make_shared<CmdGetMenu>()});
    comandos.insert({Protocolo::CMD_ADD_ORDER, std::make_shared<CmdAddOrder>()});
}

std::shared_ptr<IComandoServidor> NetworkServer::getComando(const QString &cmd) const
{
    auto it = comandos.find(cmd);
    if (it == comandos.end())
        return nullptr;
    return it->second;
}

void NetworkServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        std::cerr << "NetworkServer: fallo al aceptar descriptor de socket" << std::endl;
        socket->deleteLater();
        return;
    }

    clientes.insert(socket);

    connect(socket, &QTcpSocket::readyRead, this, &NetworkServer::onMensajeRecibido);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkServer::onClienteDesconectado);

    std::cout << "NetworkServer: cliente conectado" << std::endl;
}

void NetworkServer::onClienteDesconectado()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;
    clientes.remove(socket);
    socket->deleteLater();
    std::cout << "NetworkServer: cliente desconectado" << std::endl;
}

void NetworkServer::enviarError(QTcpSocket *socket, const QString &mensaje)
{
    QJsonObject resp;
    resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
    resp.insert(Protocolo::KEY_MSG, mensaje);
    QJsonDocument d(resp);
    if (socket && socket->isOpen())
        socket->write(d.toJson(QJsonDocument::Compact));
}

void NetworkServer::onMensajeRecibido()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    QByteArray raw = socket->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
    {
        enviarError(socket, QString("JSON malformado: %1").arg(err.errorString()));
        return;
    }

    QJsonObject obj = doc.object();
    if (!obj.contains(Protocolo::KEY_CMD) || !obj.value(Protocolo::KEY_CMD).isString())
    {
        enviarError(socket, QString("Falta campo 'cmd' o es inválido"));
        return;
    }

    QString cmd = obj.value(Protocolo::KEY_CMD).toString();
    QJsonObject payload = obj.contains(Protocolo::KEY_PAYLOAD) && obj.value(Protocolo::KEY_PAYLOAD).isObject()
                              ? obj.value(Protocolo::KEY_PAYLOAD).toObject()
                              : QJsonObject();

    auto comando = getComando(cmd);
    if (!comando)
    {
        enviarError(socket, QString("Comando desconocido: %1").arg(cmd));
        return;
    }

    if (!sistema)
    {
        enviarError(socket, QString("Servidor no tiene acceso al subsistema de negocio"));
        return;
    }

    // Despachar al comando
    comando->ejecutar(socket, payload, *sistema);
}
