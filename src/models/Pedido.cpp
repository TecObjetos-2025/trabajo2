#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/ItemPedido.h"

// =================================================================
// NUEVO INCLUDE CLAVE: Necesario para inicializar el estado del pedido
#include "models/EstadoEnCola.h"
#include <iostream> // Para mensajes de DEBUG/transiciones
// =================================================================

// VARIABLE DE CLASE
const double Pedido::IGV = 0.18;

// Constructor para asociar un cliente a un pedido
Pedido::Pedido(int id, std::shared_ptr<Cliente> cliente) : id(id), cliente(cliente) 
{
    // =================================================================
    // MODIFICADO: Inicializa el puntero de estado al primer estado concreto
    this->estadoActual = EstadoEnCola::getInstance();
    // La inicialización ': estado("EN_PROCESO")' ha sido ELIMINADA.
    // =================================================================
}

// Destructor para liberar memoria
Pedido::~Pedido() = default;

// Agregar items
void Pedido::agregarItem(std::shared_ptr<Producto> producto, int cantidad)
{
    items.push_back(std::make_unique<ItemPedido>(std::move(producto), cantidad));
}

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

// =================================================================
// NUEVO MÉTODO: Reemplazo de getEstado()
std::string Pedido::getEstadoNombre() const
{
    // Delega la consulta del nombre del estado al objeto de estado actual
    return estadoActual->getNombre(); 
}
// El método anterior 'std::string Pedido::getEstado() const' ha sido ELIMINADO.
// =================================================================

// =================================================================
// NUEVO MÉTODO: Setter de estado (usado INTERNAMENTE por las clases Estado)
void Pedido::setEstado(std::shared_ptr<EstadoPedido> nuevoEstado)
{
    std::cout << "INFO: Pedido #" << id << " transiciona de '" 
              << estadoActual->getNombre() << "' a '" << nuevoEstado->getNombre() << "'." << std::endl;
    this->estadoActual = nuevoEstado;
}
// El método anterior 'void Pedido::setEstado(const std::string &nuevoEstado)' ha sido ELIMINADO.
// =================================================================

// =================================================================
// NUEVOS MÉTODOS: Acciones que delegan en el estado actual (POLIMORFISMO)
void Pedido::avanzar()
{
    // El objeto de estado actual determina el siguiente estado.
    this->estadoActual->avanzar(this);
}

void Pedido::cancelar()
{
    // El objeto de estado actual determina si la cancelación es válida.
    this->estadoActual->cancelar(this);
}
// =================================================================


void Pedido::marcarComoPagado()
{
    // =================================================================
    // MODIFICADO: Elimina la manipulación directa del estado string.
    // NOTA: Idealmente, esta acción también debería delegarse a un método 
    // en EstadoPedido (ej: estadoActual->pagar(this)) que lleve al estado 'Pagado'.
    std::cout << "ADVERTENCIA: marcarComoPagado() llamado. Logica de estado 'Pagado' pendiente." << std::endl;
    // this->estado = "COMPLETADO"; <-- Lógica ELIMINADA
    // =================================================================
}