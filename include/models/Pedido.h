#ifndef PEDIDO_H
#define PEDIDO_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Implementación del patrón State para la gestión dinámica de estados de Pedido.
#include "core/EstadoPedido.h"

class Cliente;
class ItemPedido;
class Producto;
// Ya no es necesario 'class EstadoPedido;' porque el .h ya está incluido arriba

/**
 * @brief Clase Pedido
 * Representa un pedido realizado por un cliente en la cafetería. Utiliza el patrón State para gestionar dinámicamente el estado del pedido.
 */
class Pedido
{
private:
    int id;                                         ///< Identificador único del pedido
    std::weak_ptr<Cliente> cliente;                 ///< Referencia al cliente que realizó el pedido
    std::vector<std::unique_ptr<ItemPedido>> items; ///< Lista de ítems del pedido

    /**
     * @brief Estado actual del pedido
     * Gestionado mediante el patrón State. Permite cambiar el comportamiento del pedido según su estado concreto.
     */
    std::shared_ptr<EstadoPedido> estadoActual;

public:
    static const double IGV;

    /**
     * @brief Constructor de Pedido
     * @param id Identificador del pedido
     * @param cliente Cliente asociado al pedido
     */
    Pedido(int id, std::shared_ptr<Cliente> cliente);
    ~Pedido();

    int getId() const;
    std::shared_ptr<Cliente> getCliente() const;
    const std::vector<std::unique_ptr<ItemPedido>> &getItems() const;

    /**
     * @brief Obtiene el nombre del estado actual del pedido
     * @return Nombre descriptivo del estado
     */
    std::string getEstadoNombre() const;

    /**
     * @brief Avanza el estado del pedido según la lógica del estado actual
     * Delegado al objeto EstadoPedido correspondiente.
     */
    void avanzar();

    /**
     * @brief Cancela el pedido si el estado actual lo permite
     * Delegado al objeto EstadoPedido correspondiente.
     */
    void cancelar();

    /**
     * @brief Cambia el estado actual del pedido
     * Utilizado por los estados concretos para modificar el contexto.
     * @param nuevoEstado Nuevo estado a asignar
     */
    void setEstado(std::shared_ptr<EstadoPedido> nuevoEstado);

    void agregarItem(std::shared_ptr<Producto> producto, int cantidad);
    double calcularTotal() const;
    void marcarComoPagado();

    /**
     * @brief Sobrecarga del operador de salida para depuración
     */
    friend std::ostream &operator<<(std::ostream &os, const Pedido &pedido)
    {
        os << "Pedido ID: " << pedido.id;
        return os;
    }
};

#endif // PEDIDO_H