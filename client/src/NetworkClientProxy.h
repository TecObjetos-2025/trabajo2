#pragma once

#include <QObject>
#include <QTcpSocket>
#include <vector>
#include <memory>
#include <stdexcept>

#include "../../common/include/api/ICoreSistema.h"

class NetworkClientProxy : public ICoreSistema
{
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

private:
    QTcpSocket *socket = nullptr;
    QString host;
    quint16 port = 0;

    // Helper: send request and wait for a single framed JSON response
    QJsonObject enviarRequestYEsperarRespuesta(const QString &cmd, const QJsonObject &payload, int timeoutMs = 3000);
};
