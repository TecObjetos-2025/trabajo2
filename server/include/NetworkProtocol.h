#pragma once

#include <QTcpSocket>
#include <QJsonDocument>
#include <QDataStream>

namespace NetworkProtocol
{

    /**
     * @brief Envía un objeto JSON en el formato "length-prefixed" (4 bytes big-endian
     * que indican la longitud del payload seguido del payload JSON en UTF-8).
     */
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
            qDebug() << "NetworkProtocol::sendJson - writing" << out.size() << "bytes to" << socket;
            socket->write(out);
            socket->flush();
            // Asegurar que bytes lleguen al stack de escritura
            socket->waitForBytesWritten(200);
        }
    }

} // namespace NetworkProtocol
