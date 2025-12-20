#include "NetworkClientProxy.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>
#include <QHostAddress>
#include <QTimer>
#include <QDebug>
#include <QThread>

#include "../../common/include/api/Protocolo.h"

#include "NetworkProtocol.h"

NetworkClientProxy::NetworkClientProxy(const QString &host_, quint16 port_)
    : QObject(nullptr), host(host_), port(port_)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkClientProxy::onDatosRecibidos);

    // Si se proporcionaron host/port válidos en el constructor, intentar conexión
    if (!host_.isEmpty() && port_ != 0)
    {
        conectar(host_, port_);
    }

    // Conectar la señal errorOccurred del socket para retransmitirla hacia el UI
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError)
            {
        Q_UNUSED(socketError);
        if (socket)
        {
            QString mensaje = socket->errorString();
            qWarning() << "NetworkClientProxy: socket error:" << mensaje;
            emit errorOcurrido(mensaje);
        } });
}

NetworkClientProxy::~NetworkClientProxy()
{
    if (socket)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = nullptr;
    }
}

void NetworkClientProxy::conectar(const QString &host_, quint16 port_)
{
    host = host_;
    port = port_;

    if (!socket)
        socket = new QTcpSocket(this);

    // Evitar reconexiones si ya estamos conectando/conectados
    if (socket->state() == QAbstractSocket::ConnectingState || socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "NetworkClientProxy::conectar - ya en estado" << socket->state() << ", omitiendo";
        return;
    }

    socket->connectToHost(host, port);
    if (!socket->waitForConnected(2000))
    {
        QString msg = QString("NetworkClientProxy: no se pudo conectar a %1:%2").arg(host).arg(port);
        qWarning() << msg;
        emit errorOcurrido(msg);
        return;
    }
    else
    {
        qDebug() << "NetworkClientProxy: conectado a" << host << port;
    }

    // Conectar la señal errorOccurred del socket para retransmitirla hacia el UI
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError)
            {
        Q_UNUSED(socketError);
        if (socket)
        {
            QString mensaje = socket->errorString();
            qWarning() << "NetworkClientProxy: socket error:" << mensaje;
            emit errorOcurrido(mensaje);
        } });
}

void NetworkClientProxy::registrarObservador(std::shared_ptr<IObservadorCore> obs)
{
    observador = obs;
    qDebug() << "NetworkClientProxy::registrarObservador - observador registrado";
}

void NetworkClientProxy::removerObservador(IObservadorCore *obs)
{
    if (observador && observador.get() == obs)
    {
        observador.reset();
        qDebug() << "NetworkClientProxy::removerObservador - observador removido";
    }
    else
    {
        qDebug() << "NetworkClientProxy::removerObservador - no coincide con observador registrado";
    }
}

QJsonObject NetworkClientProxy::enviarRequestYEsperarRespuesta(const QString &cmd, const QJsonObject &payload, int timeoutMs)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState)
    {
        throw std::runtime_error("No conectado al servidor");
    }

    // Construir mensaje
    QJsonObject req;
    req.insert(Protocolo::KEY_CMD, cmd);
    req.insert(Protocolo::KEY_PAYLOAD, payload);

    // Enviar
    NetworkProtocol::sendJson(socket, req);

    // Espera asíncrona de respuesta
    if (!socket || socket->state() != QAbstractSocket::ConnectedState)
    {
        throw std::runtime_error("No conectado al servidor");
    }

    // Helper: decidir si una respuesta corresponde al comando pedido
    auto responseMatchesCmd = [cmd](const QJsonObject &r)
    {
        // Para GET_MENU y GET_ORDERS esperamos un 'data' con un array
        if (cmd == Protocolo::CMD_GET_MENU || cmd == Protocolo::CMD_GET_ORDERS)
        {
            return r.contains(Protocolo::KEY_DATA) && r.value(Protocolo::KEY_DATA).isArray();
        }
        // Para otros comandos aceptamos una respuesta con status (OK/ERROR)
        return r.contains(Protocolo::KEY_STATUS);
    };

    // Si existe una respuesta en la cola que empañe con el comando, devolverla (no consumir respuestas de otros comandos)
    for (int i = 0; i < colaRespuestas.size(); ++i)
    {
        QJsonObject cand = colaRespuestas.at(i);
        if (responseMatchesCmd(cand))
        {
            qDebug() << "NetworkClientProxy::enviarRequestYEsperarRespuesta - found queued matching response for cmd" << cmd;
            colaRespuestas.removeAt(i);
            return cand;
        }
    }

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    // Cuando llegue una respuesta, comprobamos si corresponde y salimos del loop
    QMetaObject::Connection conn = connect(this, &NetworkClientProxy::respuestaRecibida, this, [&]()
                                           {
        for (int i = 0; i < colaRespuestas.size(); ++i)
        {
            QJsonObject cand = colaRespuestas.at(i);
            if (responseMatchesCmd(cand))
            {
                qDebug() << "NetworkClientProxy::enviarRequestYEsperarRespuesta - respuesta recibida y coincide con cmd" << cmd;
                loop.quit();
                return;
            }
        }
        // Si no hay coincidencias aún, seguimos esperando
        qDebug() << "NetworkClientProxy::enviarRequestYEsperarRespuesta - respuesta recibida pero no coincide con cmd" << cmd; });

    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(timeoutMs);

    if (colaRespuestas.isEmpty())
        loop.exec();

    disconnect(conn);

    // Buscar nuevamente la respuesta coincidente (puede haber llegado mientras esperábamos)
    for (int i = 0; i < colaRespuestas.size(); ++i)
    {
        QJsonObject cand = colaRespuestas.at(i);
        if (responseMatchesCmd(cand))
        {
            qDebug() << "NetworkClientProxy::enviarRequestYEsperarRespuesta - returning matching response after wait for cmd" << cmd;
            colaRespuestas.removeAt(i);
            return cand;
        }
    }

    throw std::runtime_error("Timeout esperando respuesta JSON");
}

std::vector<InfoProducto> NetworkClientProxy::getMenu()
{
    QJsonObject payload; // vacío
    QJsonObject resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_GET_MENU, payload);
    if (!resp.contains(Protocolo::KEY_STATUS) || !resp.value(Protocolo::KEY_STATUS).isString())
    {
        throw std::runtime_error("Respuesta sin status");
    }
    QString status = resp.value(Protocolo::KEY_STATUS).toString();
    if (status != "OK")
    {
        QString msg = resp.contains(Protocolo::KEY_MSG) ? resp.value(Protocolo::KEY_MSG).toString() : QString("Error desconocido");
        throw std::runtime_error(msg.toStdString());
    }

    std::vector<InfoProducto> menu;
    if (resp.contains(Protocolo::KEY_DATA) && resp.value(Protocolo::KEY_DATA).isArray())
    {
        QJsonArray arr = resp.value(Protocolo::KEY_DATA).toArray();
        for (const QJsonValue &v : arr)
        {
            if (!v.isObject())
                continue;
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
    for (const auto &it : items)
    {
        QJsonObject o;
        o.insert("productoId", it.productoId);
        o.insert("cantidad", it.cantidad);
        arr.append(o);
    }
    payload.insert("items", arr);

    QJsonObject resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_ADD_ORDER, payload);
    if (!resp.contains(Protocolo::KEY_STATUS) || !resp.value(Protocolo::KEY_STATUS).isString())
    {
        throw std::runtime_error("Respuesta sin status");
    }
    QString status = resp.value(Protocolo::KEY_STATUS).toString();
    if (status != "OK")
    {
        QString msg = resp.contains(Protocolo::KEY_MSG) ? resp.value(Protocolo::KEY_MSG).toString() : QString("Error desconocido");
        throw std::runtime_error(msg.toStdString());
    }
}

/**
 * @brief Slot asíncrono que procesa datos entrantes del socket y extrae mensajes framed
 * emits respuestaRecibida cuando se parsea un mensaje completo
 */
void NetworkClientProxy::onDatosRecibidos()
{
    if (!socket)
        return;

    QByteArray chunk = socket->readAll();
    qDebug() << "NetworkClientProxy::onDatosRecibidos: received chunk size" << chunk.size();
    bufferAcumulado.append(chunk);

    // Procesar todos los mensajes completos en el buffer
    while (bufferAcumulado.size() >= static_cast<int>(sizeof(quint32)))
    {
        // Leer cabecera (4 bytes BE)
        QByteArray hdr = bufferAcumulado.left(static_cast<int>(sizeof(quint32)));
        QDataStream ds(hdr);
        ds.setByteOrder(QDataStream::BigEndian);
        quint32 len = 0;
        ds >> len;
        qDebug() << "NetworkClientProxy::onDatosRecibidos: next message len=" << len << " bufferSize=" << bufferAcumulado.size();

        if (bufferAcumulado.size() < static_cast<int>(sizeof(quint32) + len))
        {
            // Aún no llegó todo el cuerpo
            qDebug() << "NetworkClientProxy::onDatosRecibidos: mensaje fragmentado, esperando";
            break;
        }

        // Extraer body
        QByteArray body = bufferAcumulado.mid(static_cast<int>(sizeof(quint32)), static_cast<int>(len));
        bufferAcumulado.remove(0, static_cast<int>(sizeof(quint32) + len));

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(body, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
        {
            qWarning() << "NetworkClientProxy: JSON malformado recibido:" << err.errorString();
            continue; // intentar procesar próximos mensajes
        }

        QJsonObject obj = doc.object();
        qDebug() << "NetworkClientProxy: parsed response:" << QJsonDocument(obj).toJson(QJsonDocument::Compact);

        // Discriminación: eventos vs respuestas
        if (obj.contains(Protocolo::KEY_CMD) && obj.value(Protocolo::KEY_CMD).isString())
        {
            QString cmd = obj.value(Protocolo::KEY_CMD).toString();
            if (cmd == Protocolo::EVT_NEW_ORDER)
            {
                qDebug() << "NetworkClientProxy: recibido evento EVT_NEW_ORDER" << QJsonDocument(obj).toJson(QJsonDocument::Compact);
                if (observador)
                {
                    // Evitar llamadas reentrantes directas en el contexto del socket
                    qDebug() << "NetworkClientProxy: observador presente:" << observador.get() << "use_count=" << observador.use_count() << " - programando notificación deferred";
                    // Capturar shared_ptr para mantenerlo vivo hasta la notificación
                    auto obsCopy = observador;
                    QTimer::singleShot(50, this, [obsCopy]()
                                       {
                        if (obsCopy)
                            obsCopy->onNuevosPedidosEnCola(); });
                }
                else
                {
                    qDebug() << "NetworkClientProxy: sin observador registrado para EVT_NEW_ORDER";
                }
                // No encolamos ni emitimos respuestaRecibida: es un evento push
                continue;
            }
        }

        // Si contiene status, se asume que es respuesta a un comando
        if (obj.contains(Protocolo::KEY_STATUS))
        {
            qDebug() << "NetworkClientProxy: recibido response (status) - encolando y emitiendo señal";
            colaRespuestas.enqueue(obj);
            emit respuestaRecibida(obj);
            continue;
        }

        // Si no es ni evento ni respuesta conocida, loguear y encolar por compatibilidad
        qDebug() << "NetworkClientProxy: mensaje desconocido recibido, encolando por compatibilidad";
        colaRespuestas.enqueue(obj);
        emit respuestaRecibida(obj);
    }
}

// Test helper: inyectar bytes crudos como si vinieran del socket (para tests sin red)
void NetworkClientProxy::feedRawDataForTest(const QByteArray &data)
{
    qDebug() << "NetworkClientProxy::feedRawDataForTest - injecting" << data.size() << "bytes";
    bufferAcumulado.append(data);
    // Reutilizar la lógica de procesamiento de onDatosRecibidos sin necesidad de socket
    // Para no duplicar mucho código, simulamos que ha llegado un chunk
    while (bufferAcumulado.size() >= static_cast<int>(sizeof(quint32)))
    {
        QByteArray hdr = bufferAcumulado.left(static_cast<int>(sizeof(quint32)));
        QDataStream ds(hdr);
        ds.setByteOrder(QDataStream::BigEndian);
        quint32 len = 0;
        ds >> len;
        if (bufferAcumulado.size() < static_cast<int>(sizeof(quint32) + len))
            break;

        QByteArray body = bufferAcumulado.mid(static_cast<int>(sizeof(quint32)), static_cast<int>(len));
        bufferAcumulado.remove(0, static_cast<int>(sizeof(quint32) + len));

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(body, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
        {
            qWarning() << "NetworkClientProxy::feedRawDataForTest - JSON malformado:" << err.errorString();
            continue;
        }

        QJsonObject obj = doc.object();
        // Reusar la misma discriminación que en onDatosRecibidos
        if (obj.contains(Protocolo::KEY_CMD) && obj.value(Protocolo::KEY_CMD).isString())
        {
            QString cmd = obj.value(Protocolo::KEY_CMD).toString();
            if (cmd == Protocolo::EVT_NEW_ORDER)
            {
                if (observador)
                    observador->onNuevosPedidosEnCola();
                continue;
            }
        }
        if (obj.contains(Protocolo::KEY_STATUS))
        {
            colaRespuestas.enqueue(obj);
            emit respuestaRecibida(obj);
            continue;
        }
        colaRespuestas.enqueue(obj);
        emit respuestaRecibida(obj);
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
    qDebug() << "NetworkClientProxy::getPedidosActivos - solicitando al servidor";
    QJsonObject payload; // vacío
    QJsonObject resp;
    try
    {
        // Intentar con timeout ampliado y retry
        const int initialTimeoutMs = 5000;
        try
        {
            resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_GET_ORDERS, payload, initialTimeoutMs);
        }
        catch (const std::exception &ex)
        {
            qWarning() << "NetworkClientProxy::getPedidosActivos - error al solicitar pedidos (primer intento):" << ex.what() << " - reintentando";
            QThread::msleep(50);
            resp = enviarRequestYEsperarRespuesta(Protocolo::CMD_GET_ORDERS, payload, initialTimeoutMs);
        }
    }
    catch (const std::exception &ex)
    {
        qWarning() << "NetworkClientProxy::getPedidosActivos - error al solicitar pedidos:" << ex.what();
        return {};
    }

    if (!resp.contains(Protocolo::KEY_STATUS) || !resp.value(Protocolo::KEY_STATUS).isString())
    {
        qWarning() << "NetworkClientProxy::getPedidosActivos - respuesta sin status";
        return {};
    }
    QString status = resp.value(Protocolo::KEY_STATUS).toString();
    if (status != "OK")
    {
        QString msg = resp.contains(Protocolo::KEY_MSG) ? resp.value(Protocolo::KEY_MSG).toString() : QString("Error desconocido");
        qWarning() << "NetworkClientProxy::getPedidosActivos - status no OK:" << msg;
        return {};
    }

    std::vector<InfoPedido> pedidos;
    if (!resp.contains(Protocolo::KEY_DATA) || !resp.value(Protocolo::KEY_DATA).isArray())
    {
        qDebug() << "NetworkClientProxy::getPedidosActivos - respuesta OK pero sin data";
        return pedidos;
    }

    QJsonArray arr = resp.value(Protocolo::KEY_DATA).toArray();
    qDebug() << "NetworkClientProxy::getPedidosActivos - recibidos" << arr.size() << "pedidos";
    for (const QJsonValue &v : arr)
    {
        if (!v.isObject())
            continue;
        QJsonObject o = v.toObject();
        InfoPedido p;
        p.id_pedido = o.value("id_pedido").toInt();
        p.cliente = o.value("cliente").toString().toStdString();
        p.estado = o.value("estado").toString().toStdString();
        p.total_final = o.value("total_final").toDouble();

        if (o.contains("items") && o.value("items").isArray())
        {
            QJsonArray itemsArr = o.value("items").toArray();
            for (const QJsonValue &iv : itemsArr)
            {
                if (!iv.isObject())
                    continue;
                QJsonObject io = iv.toObject();
                ItemPedidoInfo it;
                it.nombreProducto = io.value("nombreProducto").toString().toStdString();
                it.cantidad = io.value("cantidad").toInt();
                it.precioUnitario = io.value("precioUnitario").toDouble();
                p.items.push_back(it);
            }
        }

        pedidos.push_back(p);
    }

    return pedidos;
}

void NetworkClientProxy::procesarSiguientePedido()
{
}
