#include "CmdGetMenu.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QString>

#include "core/SistemaPedidos.h"
#include "api/Protocolo.h"
#include "api/ApiDTOs.h"
#include "NetworkProtocol.h"

void CmdGetMenu::ejecutar(QTcpSocket *clientSocket, const QJsonObject & /*payload*/, SistemaPedidos &sistema)
{
    // Obtener el menú desde el subsistema
    std::vector<InfoProducto> menu = sistema.getMenu();

    QJsonArray arr;
    for (const auto &p : menu)
    {
        QJsonObject obj;
        obj.insert("id", static_cast<int>(p.id));
        obj.insert("nombre", QString::fromStdString(p.nombre));
        obj.insert("precio", p.precio);
        arr.append(obj);
    }

    QJsonObject response;
    response.insert(Protocolo::KEY_STATUS, QString("OK"));
    response.insert(Protocolo::KEY_DATA, arr);

    NetworkProtocol::sendJson(clientSocket, response);
}
