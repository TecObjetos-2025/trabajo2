#ifndef PEDIDO_H
#define PEDIDO_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "patterns/IEstrategiaDescuento.h"
#include "patterns/IEstadoPedido.h"

class Cliente;
class ItemPedido;
class Producto;

/**
 * @brief Clase Pedido
 * Representa un pedido realizado por un cliente en la cafetería. Utiliza el patrón State para gestionar dinámicamente el estado del pedido.
 */
class Pedido
{
private:
    int id;                                         ///< Identificador único del pedido
    bool pagado{false};                             ///< Indica si el pedido ha sido pagado
    std::weak_ptr<Cliente> cliente;                 ///< Referencia al cliente que realizó el pedido
    std::vector<std::unique_ptr<ItemPedido>> items; ///< Lista de ítems del pedido

    /**
     * @brief Estado actual del pedido
     * Gestionado mediante el patrón State. Permite cambiar el comportamiento del pedido según su estado concreto.
     */
    std::shared_ptr<IEstadoPedido> estadoActual;

    std::unique_ptr<IEstrategiaDescuento> estrategiaDescuento; ///< Estrategia de descuento (Strategy)

public:
    static const double IGV;

    /**
     * @brief Constructor de Pedido
     * @param id Identificador del pedido
     * @param cliente Cliente asociado al pedido
     */
    Pedido(int id, std::shared_ptr<Cliente> cliente);
    Pedido(int id, std::shared_ptr<Cliente> cliente, std::unique_ptr<IEstrategiaDescuento> estrategia);
    ~Pedido();

    int getId() const;

    bool isPagado() const { return pagado; }

    std::shared_ptr<Cliente> getCliente() const;
    const std::vector<std::unique_ptr<ItemPedido>> &getItems() const;

    /**
     * @brief Obtiene el nombre del estado actual del pedido
     * @return Nombre descriptivo del estado
     */
    std::string getEstadoNombre() const;

    /**
     * @brief Avanza el estado del pedido según la lógica del estado actual
     * Delegado al objeto IEstadoPedido correspondiente.
     */
    void avanzar();

    /**
     * @brief Cancela el pedido si el estado actual lo permite
     * Delegado al objeto IEstadoPedido correspondiente.
     */
    void cancelar();

    /**
     * @brief Cambia el estado actual del pedido
     * Utilizado por los estados concretos para modificar el contexto.
     * @param nuevoEstado Nuevo estado a asignar
     */
    void setEstado(std::shared_ptr<IEstadoPedido> nuevoEstado);

    void agregarItem(std::shared_ptr<Producto> producto, int cantidad);
    double calcularTotal() const;
    void marcarComoPagado();

    // Métodos para el patrón Strategy
    void setEstrategiaDescuento(std::unique_ptr<IEstrategiaDescuento> estrategia);
    IEstrategiaDescuento *getEstrategiaDescuento() const;

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