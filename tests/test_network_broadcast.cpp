#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QTest>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "../server/include/NetworkServer.h"
#include "../common/include/api/Protocolo.h"

TEST(NetworkServerBroadcastTest, BroadcastReceivesEvtNewOrder)
{
    int argc = 0;
    char **argv = nullptr;
    QCoreApplication app(argc, argv);

    NetworkServer server(nullptr);

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, Protocolo::DEFAULT_PORT);
    ASSERT_TRUE(client.waitForConnected(1000));

    // Dar tiempo para que se registre
    QTest::qWait(200);
    EXPECT_EQ(server.getClientesConectadosCount(), 1);

    // Llamar al handler del observador que difunde evento
    server.onNuevosPedidosEnCola();

    // Dar tiempo al event loop para procesar la emisión (especialmente en el mismo hilo)
    QTest::qWait(100);

    // Leer framing (dar un poco más de tiempo si fuera necesario)
    QByteArray buffer;
    bool ready = client.waitForReadyRead(2000);
    if (ready)
    {
        buffer.append(client.readAll());
    }
    else
    {
        // No hubo readyRead, leer lo que esté pendiente (puede haberse producido sin señal)
        QByteArray pending = client.readAll();
        qDebug() << "No readyRead; bytesAvailable=" << client.bytesAvailable() << "pending=" << pending.size();
        buffer.append(pending);
    }

    // Si no tenemos aún la cabecera, esperar un poco más
    if (buffer.size() < 4)
    {
        ASSERT_TRUE(client.waitForReadyRead(1000));
        buffer.append(client.readAll());
    }

    ASSERT_GE(buffer.size(), 4);
    QByteArray hdr = buffer.left(4);
    QDataStream ds(hdr);
    ds.setByteOrder(QDataStream::BigEndian);
    quint32 len = 0;
    ds >> len;

    // Asegurarse de tener el cuerpo completo
    while ((quint32)(buffer.size() - 4) < len)
    {
        ASSERT_TRUE(client.waitForReadyRead(1000));
        buffer.append(client.readAll());
    }

    QByteArray body = buffer.mid(4, (int)len);
    ASSERT_EQ((quint32)body.size(), len);

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(body, &err);
    ASSERT_EQ(err.error, QJsonParseError::NoError);
    ASSERT_TRUE(doc.isObject());
    QJsonObject obj = doc.object();
    ASSERT_TRUE(obj.contains(Protocolo::KEY_CMD));
    ASSERT_EQ(obj.value(Protocolo::KEY_CMD).toString(), Protocolo::EVT_NEW_ORDER);

    client.disconnectFromHost();
}
