#ifndef CORE_QT_ADAPTER_H
#define CORE_QT_ADAPTER_H

#include <QObject>
#include "api/IObservadorCore.h"
#include <string>

class CoreQtAdapter : public QObject, public IObservadorCore
{
    Q_OBJECT

public:
    explicit CoreQtAdapter(QObject *parent = nullptr);
    virtual ~CoreQtAdapter() = default;

    // Implementación de IObservadorCore

    /**
     * @brief onNuevosPedidosEnCola Notifica a la UI que la cola de pedidos ha cambiado.
     * Traduce en una señal Qt.
     */
    void onNuevosPedidosEnCola() override;

    /**
     * @brief onPedidoTerminado Notifica a la UI que un pedido específico está listo.
     * Traduce en una señal Qt.
     */
    void onPedidoTerminado(int id_pedido) override;

    /**
     * @brief onError Notifica a la UI que algo salió mal.
     * Traduce en una señal Qt.
     */
    void onError(const std::string &mensaje) override;

signals: // Señales Qt correspondientes a los métodos de IObservadorCore

    /**
     * @brief nuevosPedidosEnCola Señal emitida cuando la cola de pedidos ha cambiado.
     */
    void nuevosPedidosEnCola();

    /**
     * @brief pedidoTerminado Señal emitida cuando un pedido específico está listo.
     * @param id_pedido ID del pedido que ha sido terminado.
     */
    void pedidoTerminado(int id_pedido);

    /**
     * @brief errorOcurrido Señal emitida cuando ocurre un error.
     * @param mensaje Mensaje de error.
     */
    void errorOcurrido(const QString &mensaje);
};

#endif // CORE_QT_ADAPTER_H