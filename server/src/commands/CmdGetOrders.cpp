#include "CmdGetOrders.h"
#include "api/Protocolo.h"
#include "NetworkProtocol.h"
#include "core/SistemaPedidos.h"
#include "api/ApiDTOs.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

void CmdGetOrders::ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema)
{
    Q_UNUSED(payload);
    qDebug() << "CmdGetOrders: obteniendo pedidos activos desde el sistema";

    std::vector<InfoPedido> pedidos = sistema.getPedidosActivos();

    QJsonArray arr;
    for (const auto &p : pedidos)
    {
        QJsonObject po;
        po.insert("id_pedido", p.id_pedido);
        po.insert("cliente", QString::fromStdString(p.cliente));
        po.insert("estado", QString::fromStdString(p.estado));
        po.insert("total_final", p.total_final);

        QJsonArray itemsArr;
        for (const auto &it : p.items)
        {
            QJsonObject io;
            io.insert("nombreProducto", QString::fromStdString(it.nombreProducto));
            io.insert("cantidad", it.cantidad);
            io.insert("precioUnitario", it.precioUnitario);
            itemsArr.append(io);
        }
        po.insert("items", itemsArr);
        arr.append(po);
    }

    QJsonObject resp;
    resp.insert(Protocolo::KEY_STATUS, QString("OK"));
    resp.insert(Protocolo::KEY_DATA, arr);

    qDebug() << "CmdGetOrders: enviando respuesta con" << arr.size() << "pedidos";
    NetworkProtocol::sendJson(clientSocket, resp);
}
