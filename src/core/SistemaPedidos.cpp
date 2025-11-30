#/**
#  @file SistemaPedidos.cpp
#  @brief Implementación del sistema de gestión de pedidos para la cafetería.
#  @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
#  @date 2025
#  @details Este archivo contiene la lógica principal para la gestión de clientes, pedidos y notificaciones a la cocina.
#*/
#include "core/SistemaPedidos.h"
#include "models/Persona.h"
#include "models/Cliente.h"
#include "models/Producto.h"
#include "models/ItemPedido.h"
#include "api/ApiDTOs.h"
#include "models/Pedido.h"
#include "api/IObservadorCore.h"
#include "core/MenuCafeteria.h"
#include <iostream>
#include <stdexcept> // Capturar excepciones en la Cola
#include <algorithm> // Para std::find
#include "patterns/DescuentoNulo.h"
#include "patterns/DescuentoPorcentaje.h"
#include "patterns/DescuentoFijo.h"
#include "core/CafeteriaFactory.h"

// Función auxiliar para convertir ItemPedido a ItemPedidoInfo
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

/**
 * @brief Clase principal para la gestión de pedidos, clientes y notificaciones en la cafetería.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @details Permite registrar personas, clientes, crear pedidos y notificar observadores (patrón Observer).
 */
// Constructor
SistemaPedidos::SistemaPedidos()
{
    menu = std::make_unique<MenuCafeteria>();
}

// Destructor para liberar memoria
SistemaPedidos::~SistemaPedidos()
{
    std::cout << "Liberar memoria del sistema..." << std::endl;
    // No es necesario liberar manualmente, los punteros inteligentes lo hacen automáticamente
}

// Mejora de delegacion al mostrar el menu
void SistemaPedidos::mostrarMenu() const
{
    this->menu->mostrarMenu();
}

void SistemaPedidos::inicializarMenu()
{
    menu->agregarProducto(Producto(1, "Cafe Americano", 2.50, "Bebidas"));
    menu->agregarProducto(Producto(2, "Cafe Expresso", 3.00, "Bebidas"));
    menu->agregarProducto(Producto(3, "Pan con Chicharron", 3.20, "Desayuno"));
    menu->agregarProducto(Producto(4, "Cheesecake", 4.50, "Postres"));
    menu->agregarProducto(Producto(5, "Jugo de Naranja", 2.00, "Bebidas"));
}

// Nueva gestion
void SistemaPedidos::registrarPersona(std::shared_ptr<Persona> persona)
{
    if (persona)
    {
        personas.push_back(persona);
    }
}

Persona *SistemaPedidos::buscarPersonaPorId(int id)
{
    for (const auto &p : personas)
    {
        if (p && p->getId() == id)
        {
            return p.get();
        }
    }
    return nullptr;
}

/**
 * @brief Muestra todas las personas registradas en el sistema.
 */
void SistemaPedidos::mostrarTodasLasPersonas() const
{
    std::cout << "\n--- Lista de Personas Registradas ---" << std::endl;
    for (const auto &p : personas)
    {
        if (p)
        {
            p->mostrarInfo();
            std::cout << "----------------------" << std::endl;
        }
    }
}

// ----

/*
* Vieja gestion

void SistemaPedidos::registrarCliente(const std::string &nombre, const std::string &telefono)
{
    clientes.emplace_back(proximoIdCliente++, nombre, telefono);
    std::cout << "Cliente '" << nombre << "' registrado con exito con el ID: " << (proximoIdCliente - 1) << std::endl;
}

Cliente *SistemaPedidos::buscarClientePorId(int id)
{
    for (auto &cliente : clientes)
    {
        if (cliente.getId() == id)
        {
            return &cliente;
        }
    }
    return nullptr;
}

// Crear un nuevo pedido
Pedido &SistemaPedidos::crearPedido(Cliente &cliente)
{
    pedidos.emplace_back(proximoIdPedido++, cliente);
    return pedidos.back();
}
*/

// Agregar un producto a un pedido existente.
void SistemaPedidos::agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad)
{
    const Producto *productoRaw = menu->getProductoPorId(idProducto);
    if (productoRaw != nullptr)
    {
        // Buscar el shared_ptr correspondiente en productos
        std::shared_ptr<Producto> productoPtr;
        for (const auto &prod : productos)
        {
            if (prod && prod->getId() == idProducto)
            {
                productoPtr = std::shared_ptr<Producto>(prod.get(), [](Producto *) {}); // aliasing, no ownership
                break;
            }
        }
        if (productoPtr)
        {
            pedido.agregarItem(productoPtr, cantidad);
            std::cout << "Se agregaron " << cantidad << "x "
                      << productoRaw->getNombre()
                      << " al pedido." << std::endl;
        }
        else
        {
            std::cout << "Error: Producto con ID " << idProducto
                      << " no gestionado por el sistema." << std::endl;
        }
    }
    else
    {
        std::cout << "Hubo un error: Producto con ID "
                  << idProducto << " no encontrado en el menu." << std::endl;
    }
}

void SistemaPedidos::agregarProducto(std::shared_ptr<Producto> producto)
{
    if (producto)
    {
        productos.push_back(producto);
    }
}

void SistemaPedidos::finalizarPedido(std::shared_ptr<Pedido> pedido)
{
    if (pedido)
    {
        pedido->marcarComoPagado();
        std::cout << "Pedido #" << pedido->getId()
                  << " finalizado y marcado como PAGADO." << std::endl;

        // Agregar a la cola de pedidos en espera (NUEVO)
        std::cout << "[SISTEMA] Pedido #" << pedido->getId()
                  << " agregado a la cola de pedidos en espera." << std::endl;
        pedidos_en_espera.push(pedido);

        std::cout << "Notificando..." << std::endl;
        notificarObservadores(pedido.get());
    }
}

// Métodos Observer API
void SistemaPedidos::registrarObservador(std::shared_ptr<IObservadorCore> observador)
{
    if (observador)
        observadores.push_back(observador);
}

void SistemaPedidos::removerObservador(IObservadorCore *observador)
{
    auto it = std::find_if(
        observadores.begin(),
        observadores.end(),
        [observador](const std::shared_ptr<IObservadorCore> &ptr)
        {
            return ptr.get() == observador;
        });

    if (it != observadores.end())
    {
        observadores.erase(it);
    }
}

// Satisface la interfaz ICoreSistema (version con puntero crudo)
void SistemaPedidos::registrarObservador(IObservadorCore *observador)
{
    // No ownership transfer here; si se llama la version cruda, crear un weak wrapper no es seguro.
    // Esta implementación queda como stub para mantener compatibilidad con la interfaz antigua.
}

void SistemaPedidos::notificarObservadores(const Pedido *pedido)
{
    std::cout << "\n[SISTEMA] Notificando a "
              << observadores.size()
              << " observador(es) (API)..."
              << std::endl;

    for (const auto &obs : observadores)
    {
        if (pedido && obs)
            obs->onNuevosPedidosEnCola();
    } // Ejemplo: notificar cambio en la cola
}

// Métodos de ICoreSistema (API)
std::vector<InfoProducto> SistemaPedidos::getMenu()
{
    std::vector<InfoProducto> menuDTO;
    for (const auto &producto : productos)
    {
        InfoProducto dto;
        dto.id = producto->getId();
        dto.nombre = producto->getNombre();
        dto.precio = producto->getPrecio();
        menuDTO.push_back(dto);
    }
    return menuDTO;
}

std::vector<InfoDescuento> SistemaPedidos::getDescuentosDisponibles()
{
    std::vector<InfoDescuento> descuentos;
    descuentos.push_back({"nulo", "Sin descuento"});
    descuentos.push_back({"porcentaje_10", "Descuento del 10%"});
    descuentos.push_back({"porcentaje_20", "Descuento del 20%"});
    descuentos.push_back({"fijo_5", "Descuento fijo de $5"});
    descuentos.push_back({"fijo_10", "Descuento fijo de $10"});
    return descuentos;
}

void SistemaPedidos::finalizarPedido(
    const std::string &cliente,
    const std::vector<ItemPedidoCrear> &items,
    const std::string &id_descuentos)
{
    // Crear pedido completo usando la Factory
    auto pedido = CafeteriaFactory::crearPedidoCompleto(
        proximoIdPersona++, // ID dummy, puedes mejorar esto
        cliente,
        items,
        id_descuentos,
        personas,
        productos);
    if (!pedido)
    {
        std::cout << "[API] Cliente no encontrado: " << cliente << std::endl;
        notificarObservadores(nullptr);
        return;
    }
    pedido->marcarComoPagado();
    std::cout << "[API] Pedido finalizado y marcado como PAGADO. Total: $" << pedido->calcularTotal() << std::endl;
    pedidos_en_espera.push(pedido);
    notificarObservadores(pedido.get());
}

std::vector<InfoPedido> SistemaPedidos::getPedidosEnCola()
{
    std::vector<InfoPedido> pedidosDTO;
    std::deque<std::shared_ptr<Pedido>> snapshot = pedidos_en_espera.snapshot();

    for (const auto &pedido : snapshot)
    {
        InfoPedido dto;
        dto.id_pedido = pedido->getId();
        dto.cliente = pedido->getCliente()
                          ? pedido->getCliente()->getNombre()
                          : "";

        dto.estado = pedido->getEstadoNombre();

        dto.total_final = pedido->calcularTotal();

        for (const auto &itemPtr : pedido->getItems())
        {
            const ItemPedido *item = itemPtr.get();
            dto.items.push_back(convertirItemPedido(item));
        }
        pedidosDTO.push_back(dto);
    }

    return pedidosDTO;
}

void SistemaPedidos::procesarSiguientePedido()
{ // TODO: Procesar pedido y notificar
    notificarObservadores(nullptr);
}

/**
 * @brief Implementacion del "Consumidor" (Cocinero).
 * Desencolar el siguiente pedido en la cola y devolverlo para su procesamiento.
 */
std::shared_ptr<Pedido> SistemaPedidos::procesarSiguientePedidoInterno()
{
    std::cout << "\n[COCINERO] Buscando nuevo pedido en la cola..." << std::endl;

    try
    {
        auto pedidoProcesadoPtr = pedidos_en_espera.pop();

        if (pedidoProcesadoPtr)
        {
            std::cout << "[COCINERO] ...Pedido #"
                      << pedidoProcesadoPtr->getId()
                      << " listo para ser procesado."
                      << std::endl;
        }

        return pedidoProcesadoPtr;
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "[COCINERO] No hay pedidos en espera para procesar."
                  << std::endl;
        return nullptr;
    }
}

void SistemaPedidos::notificarPedidoTerminado(const std::shared_ptr<Pedido> &pedido)
{
    std::cout << "[SISTEMA] Notificando que el pedido #"
              << (pedido ? pedido->getId() : -1)
              << " ha sido terminado..."
              << std::endl;

    for (const auto &obs : observadores)
    {
        if (obs && pedido)
            obs->onPedidoTerminado(pedido->getId());
    }
}

/**
 * @brief Mostrar el estado actual de la cola de pedidos en espera
 */
void SistemaPedidos::mostrarPedidosEnEspera() const
{
    std::cout << "\n --- ESTADO ACTUAL DE LA COLA DE PEDIDOS EN ESPERA --- "
              << std::endl;

    pedidos_en_espera.mostrar();

    std::cout << "----------------------------------------------------- \n"
              << std::endl;
}
