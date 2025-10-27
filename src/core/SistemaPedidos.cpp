#include "core/SistemaPedidos.h"
#include "models/Persona.h"
#include "models/Producto.h"
#include "models/Pedido.h"
#include "patterns/Observador.h"
#include "core/MenuCafeteria.h"
#include <iostream>
#include <stdexcept> // Capturar excepciones en la Cola

// Constructor
SistemaPedidos::SistemaPedidos()
{
    this->menu = new MenuCafeteria();
}

// Destructor para liberar memoria
SistemaPedidos::~SistemaPedidos()
{
    std::cout << "Liberar memoria del sistema..." << std::endl;

    // De las personas
    for (Persona *p : personas)
    {
        delete p;
    }

    // De productos
    for (Producto *p : productos)
    {
        delete p;
    }

    // Vaciar la cola de pedidos y liberar memoria
    std::cout << "Liberando pedidos en espera..." << std::endl;
    while (!pedidos_en_espera.estaVacia())
    {
        Pedido *pedido = pedidos_en_espera.desencolar();
        delete pedido;
    }

    delete menu;
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
void SistemaPedidos::registrarPersona(Persona *persona)
{
    if (persona != nullptr)
    {
        personas.push_back(persona);
    }
}

Persona *SistemaPedidos::buscarPersonaPorId(int id)
{
    for (Persona *p : personas)
    {
        if (p->getId() == id)
        {
            return p;
        }
    }
    return nullptr;
}

void SistemaPedidos::mostrarTodasLasPersonas() const
{
    std::cout << "\n--- Lista de Personas Registradas ---" << std::endl;
    for (const Persona *p : personas)
    {
        p->mostrarInfo();
        std::cout << "----------------------" << std::endl;
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
    const Producto *producto = menu->getProductoPorId(idProducto);
    if (producto != nullptr)
    {
        pedido.agregarItem(producto, cantidad);
        std::cout << "Se agregaron " << cantidad << "x " << producto->getNombre() << " al pedido." << std::endl;
    }
    else
    {
        std::cout << "Hubo un error: Producto con ID " << idProducto << " no encontrado en el menu." << std::endl;
    }
}

void SistemaPedidos::agregarProducto(Producto *producto)
{
    if (producto)
    {
        productos.push_back(producto);
    }
}

void SistemaPedidos::finalizarPedido(Pedido *pedido)
{
    if (pedido)
    {
        pedido->marcarComoPagado();
        std::cout << "Pedido #" << pedido->getId()
                  << " finalizado y marcado como PAGADO." << std::endl;

        // Agregar a la cola de pedidos en espera (NUEVO)
        std::cout << "[SISTEMA] Pedido #" << pedido->getId() << " agregado a la cola de pedidos en espera." << std::endl;
        pedidos_en_espera.encolar(pedido);

        std::cout << "Notificando..." << std::endl;
        notificarObservadores(pedido);
    }
}

// Metodos del Observador
void SistemaPedidos::agregarObservador(Observador *obs)
{
    observadores.push_back(obs);
}

// Notificar a todos los observadores en la lista
void SistemaPedidos::notificarObservadores(const Pedido *pedido)
{
    std::cout << "\n[SISTEMA] Notificando a " << observadores.size() << " observador(es)..." << std::endl;
    for (auto *obs : observadores)
    {
        obs->actualizar(pedido);
    }
}

// Implementacion Practica 5: Procesar el siguiente pedido en la cola

/**
 * @brief Implementacion del "Consumidor" (Cocinero).
 * Desencolar el siguiente pedido en la cola y devolverlo para su procesamiento.
 */
Pedido *SistemaPedidos::procesarSiguientePedido()
{
    std::cout << "\n[COCINERO] Buscando nuevo pedido en la cola..." << std::endl;
    try
    {
        Pedido *pedidoProcesado = pedidos_en_espera.desencolar();

        std::cout << "[COCINERO] ...Pedido #" << pedidoProcesado->getId() << " listo para ser procesado." << std::endl;

        return pedidoProcesado;
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "[COCINERO] No hay pedidos en espera para procesar." << std::endl;
        return nullptr;
    }
}