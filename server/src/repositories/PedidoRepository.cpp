#include "repositories/PedidoRepository.h"
#include "db/DatabaseManager.h"
#include "api/ApiDTOs.h"
#include "models/ItemPedido.h"
#include "models/Producto.h"
#include "models/Cliente.h"
#include <iostream>

PedidoRepository::PedidoRepository() {}

void PedidoRepository::save(std::shared_ptr<Pedido> pedido)
{
    if (!pedido) return;

    // 1. Add to In-Memory History
    listaMaestraPedidos.push_back(pedido);

    // 2. Persist to Database (DTO Mapping Logic moved here)
    InfoPedido dto;
    dto.id_pedido = pedido->getId();
    auto cliente = pedido->getCliente();
    dto.cliente = cliente ? cliente->getNombre() : "Invitado";
    dto.estado = pedido->getEstadoNombre();
    dto.total_final = pedido->calcularTotal();

    for (const auto &item : pedido->getItems())
    {
        ItemPedidoInfo itemDto;
        if (item->getProducto())
        {
            itemDto.nombreProducto = item->getProducto()->getNombre();
            itemDto.precioUnitario = item->getProducto()->getPrecio();
        }
        itemDto.cantidad = item->getCantidad();
        dto.items.push_back(itemDto);
    }

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

void PedidoRepository::closeKitchenQueue()
{
    pedidos_en_espera.cerrar();
}

void PedidoRepository::printQueueStatus() const
{
    pedidos_en_espera.mostrar();
}
