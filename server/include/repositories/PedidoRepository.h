#pragma once

#include <vector>
#include <memory>
#include "models/Pedido.h"
#include "ColaThreadSafe.h"
#include "api/ApiDTOs.h"

class PedidoRepository
{
public:
    PedidoRepository();
    ~PedidoRepository() = default;

    /**
     * @brief Persists the order to the database and adds it to the internal history.
     * @param pedido The order to save.
     */
    void save(std::shared_ptr<Pedido> pedido);

    /**
     * @brief Adds the order to the kitchen queue for processing.
     * @param pedido The order to queue.
     */
    void queueForKitchen(std::shared_ptr<Pedido> pedido);

    /**
     * @brief Retrieves and removes the next order from the kitchen queue.
     * @return The next order, or nullptr if the queue is empty.
     */
    std::shared_ptr<Pedido> nextForKitchen();

    /**
     * @brief Returns the full history of orders managed by this repository.
     */
    std::vector<std::shared_ptr<Pedido>> getHistory() const;

    /**
     * @brief Returns the full history of orders as InfoPedido DTOs.
     */
    std::vector<InfoPedido> getHistoryDTOs() const;

    /**
     * @brief Closes the kitchen queue, preventing new items from being pushed.
     */
    void closeKitchenQueue();

    /**
     * @brief Prints the current status of the kitchen queue to stdout.
     */
    void printQueueStatus() const;

private:
    std::vector<std::shared_ptr<Pedido>> listaMaestraPedidos;
    ColaThreadSafe<std::shared_ptr<Pedido>> pedidos_en_espera;
};
