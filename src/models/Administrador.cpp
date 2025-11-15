#include "models/Administrador.h"
#include "models/Pedido.h"
#include "models/ItemPedido.h"
#include "models/Producto.h"
#include <iostream>

Administrador::Administrador(int id, const std::string &nombre, const std::string &codigoEmpleado)
    : Empleado(id, nombre, codigoEmpleado) {}

void Administrador::mostrarInfo() const
{
    std::cout << "--- Ficha de Administrador ---" << std::endl;
    std::cout << "ID: " << id << ", Nombre: " << nombre
              << ", Puesto: Administrador, Codigo: " << codigoEmpleado << std::endl;
}

void Administrador::actualizar(const Pedido *pedido)
{
    // Actualizar stats al notificar un pedido
    for (const ItemPedido *item : pedido->getItems())
    {
        ventasPorProducto[item->getProducto()->getNombre()] += item->getCantidad();
    }
}

void Administrador::mostrarEstadisticasVentas() const
{
    std::cout << "\n--- ESTADISTICAS DE VENTAS (ADMIN) ---" << std::endl;
    if (ventasPorProducto.empty())
    {
        std::cout << "No hay ventas registradas." << std::endl;
    }
    else
    {
        for (const auto &par : ventasPorProducto)
        {
            std::cout << "  - Producto: " << par.first << " | Vendidos: " << par.second << std::endl;
        }
    }
    std::cout << "--------------------------------------" << std::endl;
}

// Métodos de IObservadorCore (stubs)
void Administrador::onNuevosPedidosEnCola()
{
    std::cout << "[Administrador] Notificado: Hay nuevos pedidos en cola." << std::endl;
}

void Administrador::onPedidoTerminado(int id_pedido)
{
    std::cout << "[Administrador] Notificado: Pedido terminado, id=" << id_pedido << std::endl;
}

void Administrador::onError(const std::string &mensaje)
{
    std::cout << "[Administrador] Error: " << mensaje << std::endl;
}