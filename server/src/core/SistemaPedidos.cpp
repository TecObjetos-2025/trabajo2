/**
 *  @file SistemaPedidos.cpp
 *  @brief Implementación del sistema de gestión de pedidos para la cafetería.
 *  @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 *  @date 2025
 *  @details Este archivo contiene la lógica principal para la gestión de clientes, pedidos y notificaciones a la cocina.
 */

#include <iostream>
#include <stdexcept> // Capturar excepciones en la Cola
#include <algorithm> // Para std::find
#include <iomanip>   // Formatear salida de precios
#include "core/SistemaPedidos.h"
#include "models/Persona.h"
#include "models/Cliente.h"
#include "models/Producto.h"
#include "models/ItemPedido.h"
#include "models/Pedido.h"
#include "api/IObservadorCore.h"
#include "patterns/DescuentoNulo.h"
#include "patterns/DescuentoPorcentaje.h"
#include "patterns/DescuentoFijo.h"
#include "core/CafeteriaFactory.h"
#include "core/Configuracion.h"

/**
 * @brief Clase principal para la gestión de pedidos, clientes y notificaciones en la cafetería.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @details Permite registrar personas, clientes, crear pedidos y notificar observadores (patrón Observer).
 */
// Constructor
SistemaPedidos::SistemaPedidos(std::shared_ptr<ProductRepository> prodRepo, std::shared_ptr<PedidoRepository> orderRepo)
    : productRepository(prodRepo), pedidoRepository(orderRepo)
{
}

// Destructor para liberar memoria
SistemaPedidos::~SistemaPedidos()
{
    std::cout << "Liberar memoria del sistema..." << std::endl;
}

// Mejora de delegacion al mostrar el menu
void SistemaPedidos::mostrarMenu() const
{
    std::vector<std::shared_ptr<Producto>> allProducts = productRepository->getAll();

    std::cout << "----------- MENU DE LA CAFETERIA -----------" << std::endl;
    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(25) << "Nombre"
              << std::setw(15) << "Categoria"
              << std::right << std::setw(10) << "Precio" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    for (const auto &producto : allProducts)
    {
        std::cout << std::left << std::setw(5) << producto->getId()
                  << std::setw(25) << producto->getNombre()
                  << std::setw(15) << producto->getCategoria()
                  << std::right << std::setw(10) << producto->getPrecio() << std::endl;
    }
    std::cout << "---------------------------------------------------" << std::endl;
}

// Helper method for tests
std::vector<std::shared_ptr<Producto>> SistemaPedidos::getProductos() const
{
    return productRepository->getAll();
}

void SistemaPedidos::cerrarColaPedidos()
{
    pedidoRepository->closeKitchenQueue();
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

// Agregar un producto a un pedido existente.
void SistemaPedidos::agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad)
{
    std::shared_ptr<Producto> productoPtr = productRepository->findById(idProducto);

    if (productoPtr)
    {
        pedido.agregarItem(productoPtr, cantidad);
        std::cout << "Se agregaron " << cantidad << "x "
                  << productoPtr->getNombre()
                  << " al pedido." << std::endl;
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
        productRepository->add(producto);
    }
}

void SistemaPedidos::finalizarPedido(std::shared_ptr<Pedido> pedido)
{
    if (pedido)
    {
        pedido->marcarComoPagado();
        std::cout << "Pedido #" << pedido->getId()
                  << " finalizado y marcado como PAGADO." << std::endl;

        // Persist to DB and add to History via Repository
        pedidoRepository->save(pedido);

        // Add to Kitchen Queue via Repository
        std::cout << "[SISTEMA] Pedido #" << pedido->getId()
                  << " agregado a la cola de pedidos en espera." << std::endl;
        pedidoRepository->queueForKitchen(pedido);

        std::cout << "Notificando..." << std::endl;
        notificarObservadores(pedido.get());
    }
}

// Métodos Observer API
void SistemaPedidos::registrarObservador(std::shared_ptr<IObservadorCore> observador)
{
    if (observador)
    {
        auto it = std::find_if(
            observadores.begin(),
            observadores.end(),
            [observador](const std::shared_ptr<IObservadorCore> &ptr)
            {
                return ptr.get() == observador.get();
            });
        if (it != observadores.end())
        {
            observadores.push_back(observador);
            std::cout << "[SISTEMA] Observador ya registrado, omitiendo..." << std::endl;
            return;
        }
        else
        {
            observadores.push_back(observador);
            std::cout << "[SISTEMA] Observador registrado exitosamente." << std::endl;
        }
    }
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
    auto productos = productRepository->getAll();
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

double SistemaPedidos::getPorcentajeIGV() const
{
    return Config::TASA_IGV; // Porcentaje fijo del IGV
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
        productRepository->getAll());
    if (!pedido)
    {
        std::cout << "[API] Cliente no encontrado: " << cliente << std::endl;
        notificarObservadores(nullptr);
        return;
    }
    pedido->marcarComoPagado();
    std::cout << "[API] Pedido finalizado y marcado como PAGADO. Total: $"
              << std::fixed << std::setprecision(2)
              << pedido->calcularTotal()
              << std::endl;

    // Persist to DB and add to History via Repository
    pedidoRepository->save(pedido);

    // Add to Kitchen Queue via Repository
    pedidoRepository->queueForKitchen(pedido);

    notificarObservadores(pedido.get());
}

std::vector<InfoPedido> SistemaPedidos::getPedidosActivos()
{
    return pedidoRepository->getHistoryDTOs();
}

void SistemaPedidos::procesarSiguientePedido()
{
    auto pedido = procesarSiguientePedidoInterno();

    if (pedido)
    {
        std::cout << "[SISTEMA] Procesando pedido #" << pedido->getId() << "..." << std::endl;

        pedido->avanzar(); // Estado: En Cola -> En Preparacion

        notificarObservadores(pedido.get());

        std::thread([this, pedido]()
                    {
            std::this_thread::sleep_for(std::chrono::seconds(8)); // Simular tiempo de preparación

            std::cout << "[SIMULACION COCINA] Preparando pedido #" << pedido->getId() << "..." << std::endl;

            pedido->avanzar(); // Estado: En Preparacion -> Listo

            std::cout << "[SISTEMA] Pedido #" << pedido->getId() << " listo." << std::endl;

            notificarPedidoTerminado(pedido); })
            .detach();
    }
    else
    {
        std::cout << "[SIMULACION COCINA] No hay pedidos para procesar." << std::endl;
    }
}

/**
 * @brief Implementacion del "Consumidor" (Cocinero).
 * Desencolar el siguiente pedido en la cola y devolverlo para su procesamiento.
 */
std::shared_ptr<Pedido> SistemaPedidos::procesarSiguientePedidoInterno()
{
    std::cout << "\n[COCINERO] Buscando nuevo pedido en la cola..." << std::endl;

    std::shared_ptr<Pedido> pedido = pedidoRepository->nextForKitchen();
    if (pedido)
    {
        std::cout << "[COCINERO] Pedido #"
                  << pedido->getId()
                  << " procesando."
                  << std::endl;
        return pedido;
    }

    std::cout << "[COCINERO] No hay pedidos en espera para procesar."
              << std::endl;
    return nullptr;
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

    pedidoRepository->printQueueStatus();

    std::cout << "----------------------------------------------------- \n"
              << std::endl;
}
