#ifndef CAFETERIAFACTORY_H
#define CAFETERIAFACTORY_H

#include <memory>
#include <string>
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/Cocinero.h"
#include "models/Administrador.h"
#include "api/ICoreSistema.h"
#include "api/ApiDTOs.h"

class SistemaPedidos;

class CafeteriaFactory
{
public:
    static std::shared_ptr<Cliente> crearCliente(int id, const std::string &nombre, const std::string &telefono);
    static std::shared_ptr<Cocinero> crearCocinero(int id, const std::string &nombre, const std::string &codigo, SistemaPedidos *sistema);
    static std::shared_ptr<Administrador> crearAdministrador(int id, const std::string &nombre, const std::string &codigo);
    static std::shared_ptr<Pedido> crearPedido(int id, std::shared_ptr<Cliente> cliente);

    /**
     * @brief Crea un pedido completo con cliente, ítems y estrategia de descuento.
     * @param idPedido Identificador del pedido.
     * @param clienteNombre Nombre del cliente.
     * @param items Lista de ítems a agregar (ID y cantidad).
     * @param id_descuento Identificador de la estrategia de descuento.
     * @param personas Vector de personas registradas.
     * @param productos Vector de productos disponibles.
     * @return std::shared_ptr<Pedido> Pedido creado o nullptr si hay error.
     */
    static std::shared_ptr<Pedido> crearPedidoCompleto(
        int idPedido,
        const std::string &clienteNombre,
        const std::vector<ItemPedidoCrear> &items,
        const std::string &id_descuento,
        const std::vector<std::shared_ptr<Persona>> &personas,
        const std::vector<std::shared_ptr<Producto>> &productos);

    /**
     * @brief Obtiene una estrategia de descuento según el identificador.
     * @param id Identificador de la estrategia (ej: "porcentaje_10", "fijo_5", "nulo").
     * @return std::unique_ptr<IEstrategiaDescuento> Instancia de la estrategia correspondiente.
     */
    static std::unique_ptr<IEstrategiaDescuento> obtenerEstrategia(const std::string &id);
};

#endif