#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QTest>

#include "../server/include/NetworkServer.h"
#include "../common/include/api/Protocolo.h"

TEST(NetworkServerClientsTest, ConnectDisconnectUpdatesList)
{
    int argc = 0;
    char **argv = nullptr;
    QCoreApplication app(argc, argv);

    NetworkServer server(nullptr);

    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, Protocolo::DEFAULT_PORT);
    ASSERT_TRUE(client.waitForConnected(1000));

    QTest::qWait(200);
    EXPECT_EQ(server.getClientesConectadosCount(), 1);

    client.disconnectFromHost();
    // Puede que el socket ya esté en UnconnectedState; esperar y aceptar ambos casos
    bool disconnected = client.waitForDisconnected(1000);
    if (!disconnected)
    {
        EXPECT_EQ(client.state(), QAbstractSocket::UnconnectedState);
    }

    QTest::qWait(200);
    EXPECT_EQ(server.getClientesConectadosCount(), 0);
}
