#include <string>

/**
 * @file IObservadorCore.h
 * @brief Interfaz para que la UI observe eventos del núcleo del sistema de pedidos.
 * Esta interfaz define los métodos que la UI debe implementar para recibir
 * notificaciones sobre cambios en el estado de los pedidos.
 */

class IObservadorCore
{
public:
    virtual ~IObservadorCore() = default;

    /**
     * Notifica a la UI que la cola de pedidos ha cambiado.
     */
    virtual void onNuevosPedidosEnCola() = 0;

    /**
     * Notifica a la UI que un pedido específico está listo.
     */
    virtual void onPedidoTerminado(int id_pedido) = 0;

    /**
     * Notifica a la UI que algo salió mal.
     */
    virtual void onError(const std::string &mensaje) = 0;
};