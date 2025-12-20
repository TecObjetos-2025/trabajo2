#pragma once

#include <QJsonObject>

class QTcpSocket;
class SistemaPedidos;

/**
 * @brief Interfaz abstracta para comandos que procesa una petición del cliente.
 *
 * Cada comando implementará la lógica necesaria en el método ejecutar(), que
 * recibe el socket del cliente (para responder), el payload parseado como JSON
 * y una referencia al subsistema de pedidos para operar la lógica de negocio.
 */
class IComandoServidor
{
public:
    virtual ~IComandoServidor() = default;

    /**
     * @brief Ejecuta el comando.
     * @param clientSocket Puntero al socket del cliente (puede usarse para enviar la respuesta).
     * @param payload Objeto JSON con los parámetros de la petición ya parseados.
     * @param sistema Referencia al subsistema de negocio (facade) que maneja pedidos.
     */
    virtual void ejecutar(QTcpSocket *clientSocket, const QJsonObject &payload, SistemaPedidos &sistema) = 0;
};
