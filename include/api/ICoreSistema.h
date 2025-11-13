#include <vector>
#include "IObservadorCore.h"
#include "ApiDTOs.h"

class ICoreSistema
{
public:
    virtual ~ICoreSistema() = default;

    // --- Gestion de Observadores
    virtual void registrarObservador(IObservadorCore *observador) = 0;
    virtual void removerObservador(IObservadorCore *observador) = 0;

    // --- Funciones "Vista Cajero"

    /**
     * Obtiene el menú completo de productos disponibles.
     */
    virtual std::vector<InfoProducto> getMenu() = 0;

    /**
     * Obtiene la lista de descuentos disponibles.
     */
    virtual std::vector<InfoDescuento> getDescuentosDisponibles() = 0;

    /**
     * Finaliza y paga un pedido, encolandolo para la cocina.
     */
    virtual void finalizarPedido(const std::string &cliente,
                                 const std::vector<ItemPedidoCrear> &items,
                                 const std::vector<std::string> &descuentos) = 0;

    // --- Funciones "Vista Cocina"
    /**
     * Obtiene la lista de pedidos que están en cola para ser preparados.
     */
    virtual std::vector<InfoPedido> getPedidosEnCola() = 0;

    /**
     * Intentar procesar el siguiente pedido en cola.
     * Esto conecta la logica del 'Cocinero' y el 'std::thread'.
     */
    virtual void procesarSiguientePedido() = 0;
};