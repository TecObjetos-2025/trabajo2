#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>

#include "../client/src/NetworkClientProxy.h"
#include "../server/include/NetworkServer.h"
#include "../common/include/api/Protocolo.h"

// Simple observer stub that emits a Qt signal when notified (integration variant)
class TestObservadorIntegration : public QObject, public IObservadorCore
{
    Q_OBJECT
public:
    TestObservadorIntegration() : called(false) {}

    void onNuevosPedidosEnCola() override
    {
        called = true;
        emit notified();
    }
    void onPedidoTerminado(int /*id_pedido*/) override {}
    void onError(const std::string & /*mensaje*/) override {}

    bool called;

signals:
    void notified();
};

class TestNetworkProxyBroadcastIntegration : public QObject
{
    Q_OBJECT

private slots:
    void testBroadcastTriggersObservador();
};

void TestNetworkProxyBroadcastIntegration::testBroadcastTriggersObservador()
{
    int argc = 0;
    char **argv = nullptr;
    QCoreApplication app(argc, argv);

    // Set up a simple server socket to accept connection and send the EVT
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, Protocolo::DEFAULT_PORT));

    // Client proxy
    NetworkClientProxy proxy(QStringLiteral(""), 0);
    proxy.conectar(QStringLiteral("127.0.0.1"), Protocolo::DEFAULT_PORT);

    // Wait for server to accept the connection
    QTest::qWait(200);
    QTcpSocket *serverSideSock = nullptr;
    if (server.hasPendingConnections())
        serverSideSock = server.nextPendingConnection();
    QTRY_VERIFY(serverSideSock != nullptr);

    // Setup observer
    auto obs = std::make_shared<TestObservadorIntegration>();
    QSignalSpy spy(obs.get(), SIGNAL(notified()));
    proxy.registrarObservador(obs);

    // Construct EVT_NEW_ORDER framed message and send to client
    QJsonObject evt;
    evt.insert(Protocolo::KEY_CMD, Protocolo::EVT_NEW_ORDER);
    QJsonDocument doc(evt);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << static_cast<quint32>(payload.size());
    out.append(payload);

    serverSideSock->write(out);
    serverSideSock->flush();
    serverSideSock->waitForBytesWritten(2000);

    // Wait for the proxy to process the incoming message and notify
    QTRY_VERIFY(spy.count() >= 1);
    QVERIFY(obs->called);
}

QTEST_MAIN(TestNetworkProxyBroadcastIntegration)
#include "test_network_proxy_broadcast_integration.moc"
