#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QByteArray>
#include <QEventLoop>
#include <vector>
#include <memory>
#include <stdexcept>

#include "../../common/include/api/ICoreSistema.h"

class NetworkClientProxy : public QObject, public ICoreSistema
{
    Q_OBJECT

public:
    explicit NetworkClientProxy(const QString &host = QStringLiteral("127.0.0.1"), quint16 port = 1234);
    ~NetworkClientProxy() override;

    // ICoreSistema implementation (minimal for now)
    void registrarObservador(std::shared_ptr<IObservadorCore> observador) override;
    void removerObservador(IObservadorCore *observador) override;

    std::vector<InfoProducto> getMenu() override;
    std::vector<InfoDescuento> getDescuentosDisponibles() override;
    double getPorcentajeIGV() const override;
    void finalizarPedido(const std::string &cliente,
                         const std::vector<ItemPedidoCrear> &items,
                         const std::string &id_descuentos) override;

    std::vector<InfoPedido> getPedidosActivos() override;
    void procesarSiguientePedido() override;

signals:
    // Emitido cuando llega una respuesta JSON completa (a cualquier petición)
    void respuestaRecibida(const QJsonObject &obj);

private slots:
    // Slot asíncrono que procesa datos entrantes del socket y extrae mensajes framed
    void onDatosRecibidos();

private:
    QTcpSocket *socket = nullptr;
    QString host;
    quint16 port = 0;

    // Buffer para datos parciales recibidos
    QByteArray bufferAcumulado;

    // Cola de respuestas JSON recibidas
    QQueue<QJsonObject> colaRespuestas;

    // Observador (UI) que será notificado de eventos push (ej: EVT_NEW_ORDER)
    std::shared_ptr<IObservadorCore> observador = nullptr;

    // Enviar una petición y esperar la respuesta (bloqueante)
    QJsonObject enviarRequestYEsperarRespuesta(const QString &cmd, const QJsonObject &payload, int timeoutMs = 3000);

public:
    // Test helper: inyectar bytes crudos como si vinieran del socket (para tests sin red)
    void feedRawDataForTest(const QByteArray &data);
};
