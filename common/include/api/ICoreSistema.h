#pragma once

#include <vector>
#include <memory>
#include "IObservadorCore.h"
#include "ApiDTOs.h"

/**
 * @file ICoreSistema.h
 * @brief Interfaz para el núcleo del sistema de pedidos.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Define las operaciones principales que el núcleo del sistema debe implementar,
 * incluyendo la gestión de observadores, pedidos y el menú de productos.
 */

class ICoreSistema
{
public:
    virtual ~ICoreSistema() = default;

    // --- Gestion de Observadores
    virtual void registrarObservador(std::shared_ptr<IObservadorCore> observador) = 0;
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
     * Obtiene el porcentaje actual del IGV.
     */
    virtual double getPorcentajeIGV() const = 0;

    /**
     * Finaliza y paga un pedido, encolandolo para la cocina.
     */
    virtual void finalizarPedido(const std::string &cliente,
                                 const std::vector<ItemPedidoCrear> &items,
                                 const std::string &id_descuentos) = 0;

    // --- Funciones "Vista Cocina"
    /**
     * Obtiene la lista de pedidos que están en cola para ser preparados.
     */
    virtual std::vector<InfoPedido> getPedidosActivos() = 0;

    /**
     * Intentar procesar el siguiente pedido en cola.
     * Esto conecta la logica del 'Cocinero' y el 'std::thread'.
     */
    virtual void procesarSiguientePedido() = 0;
};