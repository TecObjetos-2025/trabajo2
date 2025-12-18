#include "NetworkClientProxy.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../common/include/api/Protocolo.h"

#include "NetworkProtocol.h"

NetworkClientProxy::NetworkClientProxy(const QString &host_, quint16 port_)
    : host(host_), port(port_)
{
    socket = new QTcpSocket();
    socket->connectToHost(host, port);
    if (!socket->waitForConnected(2000)) {
        qWarning() << "NetworkClientProxy: no se pudo conectar a" << host << port;
    }
}

NetworkClientProxy::~NetworkClientProxy()
{
    if (socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

void NetworkClientProxy::registrarObservador(std::shared_ptr<IObservadorCore> /*observador*/)
{
    // No-op: observadores serán gestionados por la UI localmente o vía push events en fases posteriores
}

void NetworkClientProxy::removerObservador(IObservadorCore * /*observador*/)
{
    // No-op
}

QJsonObject NetworkClientProxy::enviarRequestYEsperarRespuesta(const QString &cmd, const QJsonObject &payload, int timeoutMs)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        throw std::runtime_error("No conectado al servidor");
    }

    // Construir mensaje
    QJsonObject req;
    req.insert(Protocolo::KEY_CMD, cmd);
    req.insert(Protocolo::KEY_PAYLOAD, payload);

    // Enviar
    NetworkProtocol::sendJson(socket, req);

    // Leer cabecera (4 bytes big-endian)
    QByteArray hdr;
    int waited = 0;
    while (hdr.size() < static_cast<int>(sizeof(quint32)) && waited < timeoutMs) {
        if (!socket->waitForReadyRead(100)) { waited += 100; continue; }
        hdr.append(socket->read(static_cast<int>(sizeof(quint32)) - hdr.size()));
    }
    if (hdr.size() < static_cast<int>(sizeof(quint32))) {
        throw std::runtime_error("Timeout esperando cabecera de respuesta");
    }

    QDataStream ds(hdr);
    ds.setByteOrder(QDataStream::BigEndian);
    quint32 len = 0;
    ds >> len;
    qDebug() << "NetworkClientProxy: respuesta len=" << len;
    int availableNow = socket->bytesAvailable();
    qDebug() << "NetworkClientProxy: bytesAvailable after header:" << availableNow;

    QByteArray body;
        if (availableNow >= static_cast<int>(len)) {
            body = socket->read(static_cast<int>(len));
            qDebug() << "NetworkClientProxy: read full body immediately, size" << body.size();
        } else {
            waited = 0;
            while (body.size() < static_cast<int>(len) && waited < timeoutMs) {
                if (!socket->waitForReadyRead(100)) { waited += 100; continue; }
                QByteArray chunk = socket->read(static_cast<int>(len) - body.size());
                qDebug() << "NetworkClientProxy: read chunk size" << chunk.size();
                body.append(chunk);
            }
        }
    if (body.size() < static_cast<int>(len)) {
        throw std::runtime_error("Timeout esperando cuerpo de respuesta");
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(body, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        throw std::runtime_error(std::string("Respuesta JSON malformada: ") + err.errorString().toStdString());
    }

    return doc.object();
}

std::vector<InfoProducto> NetworkClientProxy::getMenu()
{
    QJsonObject payload; // vacío
    QJsonObject resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_GET_MENU, payload);
    if (!resp.contains(Protocolo::KEY_STATUS) || !resp.value(Protocolo::KEY_STATUS).isString()) {
        throw std::runtime_error("Respuesta sin status");
    }
    QString status = resp.value(Protocolo::KEY_STATUS).toString();
    if (status != "OK") {
        QString msg = resp.contains(Protocolo::KEY_MSG) ? resp.value(Protocolo::KEY_MSG).toString() : QString("Error desconocido");
        throw std::runtime_error(msg.toStdString());
    }

    std::vector<InfoProducto> menu;
    if (resp.contains(Protocolo::KEY_DATA) && resp.value(Protocolo::KEY_DATA).isArray()) {
        QJsonArray arr = resp.value(Protocolo::KEY_DATA).toArray();
        for (const QJsonValue &v : arr) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            InfoProducto p;
            p.id = o.value("id").toInt();
            p.nombre = o.value("nombre").toString().toStdString();
            p.precio = o.value("precio").toDouble();
            menu.push_back(p);
        }
    }
    return menu;
}

void NetworkClientProxy::finalizarPedido(const std::string &cliente, const std::vector<ItemPedidoCrear> &items, const std::string &id_descuentos)
{
    QJsonObject payload;
    payload.insert("cliente", QString::fromStdString(cliente));
    payload.insert("id_descuento", QString::fromStdString(id_descuentos));

    QJsonArray arr;
    for (const auto &it : items) {
        QJsonObject o;
        o.insert("productoId", it.productoId);
        o.insert("cantidad", it.cantidad);
        arr.append(o);
    }
    payload.insert("items", arr);

    QJsonObject resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_ADD_ORDER, payload);
    if (!resp.contains(Protocolo::KEY_STATUS) || !resp.value(Protocolo::KEY_STATUS).isString()) {
        throw std::runtime_error("Respuesta sin status");
    }
    QString status = resp.value(Protocolo::KEY_STATUS).toString();
    if (status != "OK") {
        QString msg = resp.contains(Protocolo::KEY_MSG) ? resp.value(Protocolo::KEY_MSG).toString() : QString("Error desconocido");
        throw std::runtime_error(msg.toStdString());
    }
}

// Minimal stubs for other methods to satisfy the interface
std::vector<InfoDescuento> NetworkClientProxy::getDescuentosDisponibles()
{
    return {};
}

double NetworkClientProxy::getPorcentajeIGV() const
{
    return 0.0;
}

std::vector<InfoPedido> NetworkClientProxy::getPedidosActivos()
{
    return {};
}

void NetworkClientProxy::procesarSiguientePedido()
{
}
