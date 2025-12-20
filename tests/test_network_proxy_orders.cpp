#include <QtTest/QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include "../client/src/NetworkClientProxy.h"
#include "../client/src/NetworkProtocol.h"
#include "../common/include/api/Protocolo.h"

class TestNetworkProxyOrders : public QObject
{
    Q_OBJECT

private slots:
    void testGetPedidosActivosRoundtrip();
};

void TestNetworkProxyOrders::testGetPedidosActivosRoundtrip()
{
    // Arrange a simple server that will accept the connection so the socket is considered connected
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));
    int port = server.serverPort();
    QTcpSocket *clientSock = nullptr;
    connect(&server, &QTcpServer::newConnection, [&]()
            { clientSock = server.nextPendingConnection(); });

    NetworkClientProxy proxy(QStringLiteral("127.0.0.1"), 0);
    proxy.conectar(QStringLiteral("127.0.0.1"), static_cast<quint16>(port));
    QTRY_VERIFY(clientSock != nullptr); // ensure server accepted the connection

    QJsonObject resp;
    resp.insert(Protocolo::KEY_STATUS, QStringLiteral("OK"));
    QJsonArray dataArr;
    QJsonObject ord;
    ord.insert("id_pedido", 42);
    ord.insert("cliente", "Test Cliente");
    ord.insert("estado", "En Cola");
    ord.insert("total_final", 12.5);
    QJsonArray itemsArr;
    QJsonObject it;
    it.insert("nombreProducto", "Cafe Americano");
    it.insert("cantidad", 1);
    it.insert("precioUnitario", 5.5);
    itemsArr.append(it);
    ord.insert("items", itemsArr);
    dataArr.append(ord);
    resp.insert(Protocolo::KEY_DATA, dataArr);

    // Construir respuesta framed y encolarla en el proxy antes de llamar al método
    QJsonDocument docResp = QJsonDocument(resp);
    QByteArray payload = docResp.toJson(QJsonDocument::Compact);
    QByteArray out;
    QDataStream ds(&out, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << static_cast<quint32>(payload.size());
    out.append(payload);

    // Inject the response directly into the proxy's buffer (simulating server reply)
    proxy.feedRawDataForTest(out);

    // Llamar a getPedidosActivos (enviarRequestYEsperarRespuesta verá la respuesta ya presente)
    auto pedidos = proxy.getPedidosActivos();
    QCOMPARE((int)pedidos.size(), 1);
    QCOMPARE(pedidos[0].id_pedido, 42);
    QCOMPARE(QString::fromStdString(pedidos[0].cliente), QString("Test Cliente"));
    QCOMPARE(QString::fromStdString(pedidos[0].estado), QString("En Cola"));
    QCOMPARE((int)pedidos[0].items.size(), 1);
    QCOMPARE(QString::fromStdString(pedidos[0].items[0].nombreProducto), QString("Cafe Americano"));
}

QTEST_MAIN(TestNetworkProxyOrders)
#include "test_network_proxy_orders.moc"
