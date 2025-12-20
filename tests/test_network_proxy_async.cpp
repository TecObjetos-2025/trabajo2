#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>

#include "../client/src/NetworkClientProxy.h"
#include "../client/src/NetworkProtocol.h"
#include "../common/include/api/Protocolo.h"

// Simple observer stub that emits a Qt signal when notified
class TestObservador : public QObject, public IObservadorCore
{
    Q_OBJECT
public:
    TestObservador() : called(false) {}

    void onNuevosPedidosEnCola() override
    {
        called = true;
        emit notified();
    }
    void onPedidoTerminado(int /*id_pedido*/) override {}
    void onError(const std::string &/*mensaje*/) override {}

    bool called;

signals:
    void notified();
};

class TestNetworkProxyAsync : public QObject
{
    Q_OBJECT

private slots:
    void testRespuestaRecibida();
    void testEvtNewOrder();
};

void TestNetworkProxyAsync::testRespuestaRecibida()
{
    // Crear proxy (sin servidor real, usaremos feedRawDataForTest)
    NetworkClientProxy proxy(QStringLiteral("127.0.0.1"), 12345);

    // Preparar spy
    QSignalSpy spy(&proxy, SIGNAL(respuestaRecibida(QJsonObject)));

    // Construir mensaje framed (4B BE + JSON)
    QJsonObject resp;
    resp.insert(Protocolo::KEY_STATUS, QStringLiteral("OK"));
    resp.insert(Protocolo::KEY_DATA, QJsonArray());

    QJsonDocument doc(resp);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << static_cast<quint32>(payload.size());
    out.append(payload);

    // Inyectar bytes y verificar emisión de señal
    proxy.feedRawDataForTest(out);

    QTRY_VERIFY(spy.count() >= 1);

    // Verificar que el JSON recibido contiene status OK
    QVariantList args = spy.takeFirst();
    QJsonObject obj = args.at(0).value<QJsonObject>();
    QCOMPARE(obj.value(Protocolo::KEY_STATUS).toString(), QStringLiteral("OK"));
}

void TestNetworkProxyAsync::testEvtNewOrder()
{
    NetworkClientProxy proxy(QStringLiteral("127.0.0.1"), 12345);

    // Registrar observador stub
    auto obs = std::make_shared<TestObservador>();
    QSignalSpy spyObs(obs.get(), SIGNAL(notified()));
    proxy.registrarObservador(obs);

    // Construir evento framed
    QJsonObject evt;
    evt.insert(Protocolo::KEY_CMD, Protocolo::EVT_NEW_ORDER);
    QJsonDocument doc(evt);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << static_cast<quint32>(payload.size());
    out.append(payload);

    // Inyectar evento
    proxy.feedRawDataForTest(out);

    // Esperar a que el observador sea notificado
    QTRY_VERIFY(spyObs.count() >= 1);
    QVERIFY(obs->called);
}

QTEST_MAIN(TestNetworkProxyAsync)
#include "test_network_proxy_async.moc"
