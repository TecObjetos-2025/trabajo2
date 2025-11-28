
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/ItemPedido.h"
#include "models/EstadoEnCola.h" // Estado inicial del pedido
#include <iostream>              // Para mensajes informativos de transición de estado

// Valor del IGV aplicado a los pedidos
const double Pedido::IGV = 0.18;

/**
 * @brief Constructor de Pedido
 * Inicializa el pedido con el estado "En Cola" mediante el patrón State.
 * @param id Identificador único del pedido
 * @param cliente Cliente asociado al pedido
 */
Pedido::Pedido(int id, std::shared_ptr<Cliente> cliente)
    : id(id), cliente(cliente)
{
    // Inicializa el estado al primer estado concreto (En Cola)
    this->estadoActual = EstadoEnCola::getInstance();
}

/**
 * @brief Destructor de Pedido
 */
Pedido::~Pedido() = default;

/**
 * @brief Agrega un ítem al pedido
 * @param producto Producto a agregar
 * @param cantidad Cantidad solicitada
 */
void Pedido::agregarItem(std::shared_ptr<Producto> producto, int cantidad)
{
    items.push_back(std::make_unique<ItemPedido>(std::move(producto), cantidad));
}

/**
 * @brief Calcula el total del pedido incluyendo IGV
 * @return Total a pagar
 */
double Pedido::calcularTotal() const
{
    double subtotal = 0.0;
    for (const auto &item : items)
    {
        subtotal += item->getSubtotal();
    }
    return subtotal * (1 + IGV);
}

// Getters

int Pedido::getId() const
{
    return id;
}

std::shared_ptr<Cliente> Pedido::getCliente() const
{
    return cliente.lock();
}

const std::vector<std::unique_ptr<ItemPedido>> &Pedido::getItems() const
{
    return items;
}

/**
 * @brief Obtiene el nombre del estado actual del pedido
 * @return Nombre descriptivo del estado
 */
std::string Pedido::getEstadoNombre() const
{
    // Delega la consulta del nombre al objeto de estado actual
    return estadoActual->getNombre();
}

/**
 * @brief Cambia el estado actual del pedido
 * Utilizado internamente por los estados concretos para modificar el contexto.
 * @param nuevoEstado Nuevo estado a asignar
 */
void Pedido::setEstado(std::shared_ptr<EstadoPedido> nuevoEstado)
{
    std::cout << "INFO: Pedido #" << id << " transiciona de '"
              << estadoActual->getNombre() << "' a '" << nuevoEstado->getNombre() << "'." << std::endl;
    this->estadoActual = nuevoEstado;
}

/**
 * @brief Avanza el estado del pedido según la lógica del estado actual
 * Delegado al objeto EstadoPedido correspondiente.
 */
void Pedido::avanzar()
{
    this->estadoActual->avanzar(this);
}

/**
 * @brief Cancela el pedido si el estado actual lo permite
 * Delegado al objeto EstadoPedido correspondiente.
 */
void Pedido::cancelar()
{
    this->estadoActual->cancelar(this);
}

/**
 * @brief Marca el pedido como pagado
 * NOTA: La lógica de transición al estado "Pagado" debería implementarse en el patrón State.
 */
void Pedido::marcarComoPagado()
{
    std::cout << "ADVERTENCIA: marcarComoPagado() llamado. Lógica de estado 'Pagado' pendiente." << std::endl;
}