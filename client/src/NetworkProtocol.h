#pragma once

#include <QTcpSocket>
#include <QJsonDocument>
#include <QDataStream>

namespace NetworkProtocol
{
    inline void sendJson(QTcpSocket *socket, const QJsonObject &obj)
    {
        if (!socket)
            return;
        QJsonDocument d(obj);
        QByteArray payload = d.toJson(QJsonDocument::Compact);

        QByteArray out;
        QDataStream ds(&out, QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::BigEndian);
        ds << static_cast<quint32>(payload.size());
        out.append(payload);

        if (socket->isOpen())
        {
            socket->write(out);
            socket->flush();
        }
    }

} // namespace NetworkProtocol
