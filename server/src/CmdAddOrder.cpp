#include "commands/CmdAddOrder.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QString>

#include "core/SistemaPedidos.h"
#include "api/Protocolo.h"
#include "api/ApiDTOs.h"
#include "NetworkProtocol.h"

void CmdAddOrder::ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema)
{
    // Validación básica del payload
    if (!payload.contains("cliente") || !payload.value("cliente").isString())
    {
        QJsonObject resp;
        resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
        resp.insert(Protocolo::KEY_MSG, QString("Campo 'cliente' faltante o inválido"));
        NetworkProtocol::sendJson(clientSocket, resp);
        return;
    }

    if (!payload.contains("items") || !payload.value("items").isArray())
    {
        QJsonObject resp;
        resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
        resp.insert(Protocolo::KEY_MSG, QString("Campo 'items' faltante o inválido"));
        NetworkProtocol::sendJson(clientSocket, resp);
        return;
    }

    QString clienteQ = payload.value("cliente").toString();
    std::string cliente = clienteQ.toStdString();

    std::string id_descuento = "nulo"; // default
    if (payload.contains("id_descuento") && payload.value("id_descuento").isString())
    {
        id_descuento = payload.value("id_descuento").toString().toStdString();
    }

    QJsonArray itemsArray = payload.value("items").toArray();
    if (itemsArray.isEmpty())
    {
        QJsonObject resp;
        resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
        resp.insert(Protocolo::KEY_MSG, QString("La lista 'items' está vacía"));
        QJsonDocument d(resp);
        if (clientSocket && clientSocket->isOpen())
            clientSocket->write(d.toJson(QJsonDocument::Compact));
        return;
    }

    std::vector<ItemPedidoCrear> items;
    for (const QJsonValue &v : itemsArray)
    {
        if (!v.isObject())
        {
            QJsonObject resp;
            resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
            resp.insert(Protocolo::KEY_MSG, QString("Cada item debe ser un objeto con 'productoId' y 'cantidad'"));
            NetworkProtocol::sendJson(clientSocket, resp);
            return;
        }
        QJsonObject itm = v.toObject();

        if (!itm.contains("productoId") || !itm.value("productoId").isDouble() || !itm.contains("cantidad") || !itm.value("cantidad").isDouble())
        {
            QJsonObject resp;
            resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
            resp.insert(Protocolo::KEY_MSG, QString("Items inválidos: se requiere 'productoId' y 'cantidad' numéricos"));
            NetworkProtocol::sendJson(clientSocket, resp);
            return;
        }

        ItemPedidoCrear item;
        item.productoId = itm.value("productoId").toInt();
        item.cantidad = itm.value("cantidad").toInt();
        if (item.cantidad <= 0)
        {
            QJsonObject resp;
            resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
            resp.insert(Protocolo::KEY_MSG, QString("La 'cantidad' debe ser mayor que 0"));
            QJsonDocument d(resp);
            if (clientSocket && clientSocket->isOpen())
                clientSocket->write(d.toJson(QJsonDocument::Compact));
            return;
        }

        items.push_back(item);
    }

    // Invocar la API del sistema
    try
    {
        sistema.finalizarPedido(cliente, items, id_descuento);

        QJsonObject resp;
        resp.insert(Protocolo::KEY_STATUS, QString("OK"));
        resp.insert(Protocolo::KEY_MSG, QString("Pedido recibido y encolado"));
        NetworkProtocol::sendJson(clientSocket, resp);
    }
    catch (const std::exception &ex)
    {
        QJsonObject resp;
        resp.insert(Protocolo::KEY_STATUS, QString("ERROR"));
        resp.insert(Protocolo::KEY_MSG, QString::fromStdString(ex.what()));
        NetworkProtocol::sendJson(clientSocket, resp);
    }
}
