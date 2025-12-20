#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>

#include "../client/src/NetworkClientProxy.h"
#include "../server/include/NetworkServer.h"
#include "../server/include/core/SistemaPedidos.h"
#include "../server/include/repositories/ProductRepository.h"
#include "../server/include/repositories/PedidoRepository.h"
#include "../server/include/repositories/ClienteRepository.h"
#include "../common/include/api/Protocolo.h"

// Observer stub for integration
class TestObsEO : public QObject, public IObservadorCore
{
    Q_OBJECT
public:
    TestObsEO() : called(false) {}
    void onNuevosPedidosEnCola() override
    {
        called = true;
        emit notified();
    }
    void onPedidoTerminado(int) override {}
    void onError(const std::string &) override {}
    bool called;
signals:
    void notified();
};

class TestNetworkProxyEndToEnd : public QObject
{
    Q_OBJECT
private slots:
    void testAddOrderTriggersBroadcast();
};

void TestNetworkProxyEndToEnd::testAddOrderTriggersBroadcast()
{
    int argc = 0;
    char **argv = nullptr;
    QCoreApplication app(argc, argv);

    // Sistema y servidor (crear repositorios requeridos)
    auto repo = std::make_shared<ProductRepository>();
    auto orderRepo = std::make_shared<PedidoRepository>();
    auto clienteRepo = std::make_shared<repos::ClienteRepository>();
    SistemaPedidos sistema(repo, orderRepo, clienteRepo);
    NetworkServer server(&sistema);

    // Registrar al servidor como observador del sistema (sin ownership)
    sistema.registrarObservador(std::shared_ptr<IObservadorCore>(&server, [](IObservadorCore *) {}));

    // Clientes/proxies
    NetworkClientProxy proxy1(QStringLiteral(""), 0);
    NetworkClientProxy proxy2(QStringLiteral(""), 0);

    proxy1.conectar(QStringLiteral("127.0.0.1"), Protocolo::DEFAULT_PORT);
    proxy2.conectar(QStringLiteral("127.0.0.1"), Protocolo::DEFAULT_PORT);

    // Esperar conexiones aceptadas
    QTest::qWait(200);
    QTRY_VERIFY(server.getClientesConectadosCount() >= 2);

    // Registrar observador en proxy2 y espiarlo
    auto obs = std::make_shared<TestObsEO>();
    QSignalSpy spy(obs.get(), SIGNAL(notified()));
    proxy2.registrarObservador(obs);

    // Construir pedido simple para enviar con proxy1
    std::vector<ItemPedidoCrear> items;
    ItemPedidoCrear it;
    it.productoId = 1;
    it.cantidad = 1;
    items.push_back(it);

    // Antes de enviar, clear any potential queued responses
    // Enviar pedido (esto bloqueará hasta recibir respuesta OK o timeout)
    proxy1.finalizarPedido("Cliente Test", items, "");

    // Ahora el servidor debe haber notificado y difundir EVT_NEW_ORDER
    QTRY_VERIFY(spy.count() >= 1);
    QVERIFY(obs->called);
}

QTEST_MAIN(TestNetworkProxyEndToEnd)
#include "test_network_proxy_endtoend.moc"
