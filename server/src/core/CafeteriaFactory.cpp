/**
 *  @file CafeteriaFactory.cpp
 *  @brief Implementa la Factory para la creación centralizada de objetos del sistema de cafetería.
 *  @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 *  @date 2025
 *  @details Centraliza la gestión de punteros inteligentes y desacopla la lógica de creación de objetos complejos.
 */
#include "patterns/DescuentoNulo.h"
#include "patterns/DescuentoPorcentaje.h"
#include "patterns/DescuentoFijo.h"
#include <map>
#include <functional>
#include "core/CafeteriaFactory.h"
#include "core/SistemaPedidos.h"

/**
 * @brief Obtiene una estrategia de descuento según el identificador.
 * @param id Identificador de la estrategia (ej: "porcentaje_10", "fijo_5", "nulo").
 * @return std::unique_ptr<IEstrategiaDescuento> Instancia de la estrategia correspondiente.
 */
std::unique_ptr<IEstrategiaDescuento> CafeteriaFactory::obtenerEstrategia(const std::string &id)
{
    static const std::map<std::string, std::function<std::unique_ptr<IEstrategiaDescuento>()>> estrategias = {
        {"nulo", []()
         { return std::make_unique<DescuentoNulo>(); }},
        {"porcentaje_10", []()
         { return std::make_unique<DescuentoPorcentaje>(0.10); }},
        {"porcentaje_20", []()
         { return std::make_unique<DescuentoPorcentaje>(0.20); }},
        {"fijo_5", []()
         { return std::make_unique<DescuentoFijo>(5.0); }},
        {"fijo_10", []()
         { return std::make_unique<DescuentoFijo>(10.0); }}};
    auto it = estrategias.find(id);
    if (it != estrategias.end())
    {
        return it->second();
    }
    // Por defecto, estrategia nula
    return std::make_unique<DescuentoNulo>();
}

std::shared_ptr<Cliente> CafeteriaFactory::crearCliente(int id, const std::string &nombre, const std::string &telefono)
{
    /**
     * @brief Crea un objeto Cliente.
     * @param id Identificador único del cliente.
     * @param nombre Nombre del cliente.&
     * @param telefono Teléfono del cliente.
     * @return std::shared_ptr<Cliente> Instancia del cliente creada.
     */
    return std::make_shared<Cliente>(id, nombre, telefono);
}

std::shared_ptr<Cocinero> CafeteriaFactory::crearCocinero(int id, const std::string &nombre, const std::string &codigo, SistemaPedidos *sistema)
{
    /**
     * @brief Crea un objeto Cocinero.
     * @param id Identificador único del cocinero.
     * @param nombre Nombre del cocinero.
     * @param codigo Código interno del cocinero.
     * @param sistema Puntero al sistema de pedidos asociado.
     * @return std::shared_ptr<Cocinero> Instancia del cocinero creada.
     */
    return std::make_shared<Cocinero>(id, nombre, codigo, sistema);
}

std::shared_ptr<Administrador> CafeteriaFactory::crearAdministrador(int id, const std::string &nombre, const std::string &codigo)
{
    /**
     * @brief Crea un objeto Administrador.
     * @param id Identificador único del administrador.
     * @param nombre Nombre del administrador.
     * @param codigo Código interno del administrador.
     * @return std::shared_ptr<Administrador> Instancia del administrador creada.
     */
    return std::make_shared<Administrador>(id, nombre, codigo);
}

std::shared_ptr<Pedido> CafeteriaFactory::crearPedido(int id, std::shared_ptr<Cliente> cliente)
{
    /**
     * @brief Crea un objeto Pedido con estado inicial "En Cola".
     * @param id Identificador único del pedido.
     * @param cliente Cliente asociado al pedido.
     * @return std::shared_ptr<Pedido> Instancia del pedido creada.
     */
    return std::make_shared<Pedido>(id, cliente);
}

namespace
{
    std::shared_ptr<Cliente> buscarClientePorNombre(const std::vector<std::shared_ptr<Persona>> &personas, const std::string &nombre)
    {
        auto it = std::find_if(personas.begin(), personas.end(), [&](const std::shared_ptr<Persona> &p)
                               {
            auto cliente = std::dynamic_pointer_cast<Cliente>(p);
            return cliente && cliente->getNombre() == nombre; });
        return (it != personas.end()) ? std::dynamic_pointer_cast<Cliente>(*it) : nullptr;
    }

    std::shared_ptr<Producto> buscarProductoPorId(const std::vector<std::shared_ptr<Producto>> &productos, int productoId)
    {
        auto it = std::find_if(productos.begin(), productos.end(), [&](const std::shared_ptr<Producto> &prod)
                               { return prod && prod->getId() == productoId; });
        // No duplicar ownership, solo compartir el puntero existente
        return (it != productos.end()) ? *it : nullptr;
    }
}

std::shared_ptr<Pedido> CafeteriaFactory::crearPedidoCompleto(
    int idPedido,
    const std::string &clienteNombre,
    const std::vector<ItemPedidoCrear> &items,
    const std::string &id_descuento,
    const std::vector<std::shared_ptr<Persona>> &personas,
    const std::vector<std::shared_ptr<Producto>> &productos)
{
    auto clientePtr = buscarClientePorNombre(personas, clienteNombre);
    if (!clientePtr)
        return nullptr;

    auto pedido = crearPedido(idPedido, clientePtr);
    for (const auto &item : items)
    {
        auto prodPtr = buscarProductoPorId(productos, item.productoId);
        if (prodPtr)
            pedido->agregarItem(prodPtr, item.cantidad);
    }
    pedido->setEstrategiaDescuento(obtenerEstrategia(id_descuento));
    return pedido;
}