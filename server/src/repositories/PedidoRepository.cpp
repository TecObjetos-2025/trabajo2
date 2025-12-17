#include "repositories/PedidoRepository.h"
#include "db/DatabaseManager.h"
#include "models/ItemPedido.h"
#include "models/Producto.h"
#include "models/Cliente.h"
#include <iostream>

// Funciones auxiliares
// Convertir ItemPedido a ItemPedidoInfo
static ItemPedidoInfo convertirItemPedido(const ItemPedido *item)
{
    ItemPedidoInfo dto;
    if (item && item->getProducto())
    {
        dto.nombreProducto = item->getProducto()->getNombre();
        dto.cantidad = item->getCantidad();
        dto.precioUnitario = item->getProducto()->getPrecio();
    }
    return dto;
}

// Convertir Pedido a InfoPedido
static InfoPedido convertirPedido(const Pedido *pedido)
{
    InfoPedido dto;
    if (pedido)
    {
        dto.id_pedido = pedido->getId();
        dto.cliente = pedido->getCliente()
                          ? pedido->getCliente()->getNombre()
                          : "Invitado";

        dto.estado = pedido->getEstadoNombre();

        dto.total_final = pedido->calcularTotal();

        for (const auto &itemPtr : pedido->getItems())
        {
            const ItemPedido *item = itemPtr.get();
            dto.items.push_back(convertirItemPedido(item));
        }
    }
    return dto;
}

PedidoRepository::PedidoRepository() {}

void PedidoRepository::save(std::shared_ptr<Pedido> pedido)
{
    if (!pedido)
        return;

    // 1. Add to In-Memory History
    listaMaestraPedidos.push_back(pedido);

    // 2. Persist to Database (DTO Mapping Logic moved here)
    InfoPedido dto = convertirPedido(pedido.get());

    DatabaseManager::instance().savePedido(dto);
}

void PedidoRepository::queueForKitchen(std::shared_ptr<Pedido> pedido)
{
    if (pedido)
    {
        std::cout << "[PedidoRepository] Adding Order #" << pedido->getId()
                  << " to kitchen queue." << std::endl;
        pedidos_en_espera.push(pedido);
    }
}

std::shared_ptr<Pedido> PedidoRepository::nextForKitchen()
{
    std::shared_ptr<Pedido> pedido;
    if (pedidos_en_espera.tryPop(pedido))
    {
        return pedido;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Pedido>> PedidoRepository::getHistory() const
{
    return listaMaestraPedidos;
}

std::vector<InfoPedido> PedidoRepository::getHistoryDTOs() const
{
    std::vector<InfoPedido> dtos;
    for (const auto &pedido : listaMaestraPedidos)
    {
        dtos.push_back(convertirPedido(pedido.get()));
    }
    return dtos;
}

void PedidoRepository::closeKitchenQueue()
{
    pedidos_en_espera.cerrar();
}

void PedidoRepository::printQueueStatus() const
{
    pedidos_en_espera.mostrar();
}
