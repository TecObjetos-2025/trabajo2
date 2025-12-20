#include "NetworkServer.h"

#include "commands/CmdGetMenu.h"
#include "commands/CmdAddOrder.h"
#include "api/Protocolo.h"

#include <QHostAddress>
#include <iostream>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "NetworkProtocol.h"

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

    // Añadir a la lista de conectados y a estructuras internas
    clientesConectados.append(socket);

    qDebug() << "NetworkServer: cliente conectado, total=" << clientesConectados.count();

    connect(socket, &QTcpSocket::readyRead, this, &NetworkServer::onMensajeRecibido);
    // Cuando el cliente se desconecte, removerlo de la lista de conectados
    connect(socket, &QTcpSocket::disconnected, this, [this]()
            { this->onClienteDesconectado(); });
    // También conectar al slot que borra y limpia la entrada del buffer
    connect(socket, &QTcpSocket::disconnected, this, [this, socket]()
            { this->removerCliente(socket); });
}

void NetworkServer::onClienteDesconectado()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
    {
        qDebug() << "NetworkServer::onClienteDesconectado - sender() no es QTcpSocket*";
        return;
    }

    qDebug() << "NetworkServer: onClienteDesconectado() recibido para socket" << socket;

    // Limpieza básica: quitar buffer y eliminar socket si sigue válido
    recvBuffers.remove(socket);
    socket->deleteLater();
    qDebug() << "NetworkServer: cliente desconectado y recursos liberados";
}

void NetworkServer::removerCliente(QTcpSocket *socket)
{
    if (!socket)
    {
        qDebug() << "NetworkServer::removerCliente - socket nulo";
        return;
    }

    // Quitar de la lista de conectados
    int idx = clientesConectados.indexOf(socket);
    if (idx != -1)
    {
        clientesConectados.removeAt(idx);
        qDebug() << "NetworkServer: removerCliente - socket removido, total=" << clientesConectados.count();
    }
    else
    {
        qDebug() << "NetworkServer: removerCliente - socket no encontrado en la lista";
    }
}

void NetworkServer::enviarError(QTcpSocket *socket, const QString &mensaje)
{
    QJsonObject resp;
    resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
    resp.insert(Protocolo::KEY_MSG, mensaje);
    NetworkProtocol::sendJson(socket, resp);
}

void NetworkServer::difundirEvento(const std::string &jsonEvento)
{
    // Intentar parsear el string JSON para enviar como QJsonObject
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(jsonEvento), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
    {
        qDebug() << "NetworkServer::difundirEvento - JSON inválido:" << err.errorString();
        return;
    }
    QJsonObject obj = doc.object();

    qDebug() << "NetworkServer::difundirEvento - enviando evento a" << clientesConectados.count() << "clientes";
    for (QTcpSocket *sock : clientesConectados)
    {
        if (!sock)
            continue;
        qDebug() << "NetworkServer::difundirEvento - sock state=" << sock->state();
        if (sock->state() == QAbstractSocket::ConnectedState)
        {
            NetworkProtocol::sendJson(sock, obj);
            qDebug() << "NetworkServer::difundirEvento - evento enviado a" << sock;
        }
        else
        {
            qDebug() << "NetworkServer::difundirEvento - socket no conectado, state=" << sock->state();
        }
    }
}

void NetworkServer::onNuevosPedidosEnCola()
{
    // Construir JSON simple para notificar
    QJsonObject evt;
    evt.insert(Protocolo::KEY_CMD, Protocolo::EVT_NEW_ORDER);
    // payload vacío por ahora

    QJsonDocument doc(evt);
    std::string s = doc.toJson(QJsonDocument::Compact).toStdString();
    difundirEvento(s);
}

void NetworkServer::onMensajeRecibido()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    // Append received bytes to per-socket buffer
    QByteArray &buf = recvBuffers[socket];
    buf.append(socket->readAll());

    // Process all complete messages (length-prefixed with 4 bytes Big Endian)
    while (true)
    {
        if (buf.size() < static_cast<int>(sizeof(quint32)))
            break;

        QDataStream ds(buf);
        ds.setByteOrder(QDataStream::BigEndian);
        quint32 msgLen = 0;
        ds >> msgLen; // reads first 4 bytes

        if (buf.size() < static_cast<int>(sizeof(quint32) + msgLen))
            break; // wait for full message

        // Extract message bytes
        QByteArray msgBytes = buf.mid(sizeof(quint32), msgLen);

        // Remove processed bytes from buffer
        buf.remove(0, sizeof(quint32) + msgLen);

        // Parse JSON message
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(msgBytes, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
        {
            qDebug() << "NetworkServer: JSON parse error:" << err.errorString();
            enviarError(socket, QString("JSON malformado: %1").arg(err.errorString()));
            continue; // try next message
        }

        QJsonObject obj = doc.object();
        if (!obj.contains(Protocolo::KEY_CMD) || !obj.value(Protocolo::KEY_CMD).isString())
        {
            enviarError(socket, QString("Falta campo 'cmd' o es inválido"));
            continue;
        }

        QString cmd = obj.value(Protocolo::KEY_CMD).toString();
        qDebug() << "NetworkServer: procesando comando" << cmd;
        QJsonObject payload = obj.contains(Protocolo::KEY_PAYLOAD) && obj.value(Protocolo::KEY_PAYLOAD).isObject()
                                  ? obj.value(Protocolo::KEY_PAYLOAD).toObject()
                                  : QJsonObject();

        auto comando = getComando(cmd);
        if (!comando)
        {
            enviarError(socket, QString("Comando desconocido: %1").arg(cmd));
            continue;
        }

        if (!sistema)
        {
            enviarError(socket, QString("Servidor no tiene acceso al subsistema de negocio"));
            continue;
        }

        // Despachar al comando
        comando->ejecutar(socket, payload, *sistema);
    }
}
// For Qt's moc when building in tests (ensures meta-object is linked into this TU)
#include "NetworkServer.moc"