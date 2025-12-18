#pragma once

#include <QTcpServer>
#include <QSet>
#include <QHash>
#include <map>
#include <memory>
#include <QString>

class IComandoServidor;
class SistemaPedidos;

/**
 * @brief Servidor de red que escucha conexiones TCP y mantiene un registro de comandos.
 *
 * Además de escuchar, mantiene la lista de clientes conectados y despacha
 * mensajes entrantes a las implementaciones de `IComandoServidor`.
 */
class NetworkServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit NetworkServer(SistemaPedidos *sistema, QObject *parent = nullptr);
    ~NetworkServer() override = default;

    // Obtiene el comando registrado por nombre, o nullptr si no existe.
    std::shared_ptr<IComandoServidor> getComando(const QString &cmd) const;

protected:
    // Sobrescribir para aceptar nuevas conexiones
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onMensajeRecibido();
    void onClienteDesconectado();

private:
    void enviarError(QTcpSocket *socket, const QString &mensaje);

    std::map<QString, std::shared_ptr<IComandoServidor>> comandos;
    QSet<QTcpSocket *> clientes;
    QHash<QTcpSocket *, QByteArray> recvBuffers;
    SistemaPedidos *sistema = nullptr; // no owned
};
