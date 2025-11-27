#ifndef PEDIDO_H
#define PEDIDO_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// ==========================================
// NUEVO INCLUDE NECESARIO para el Patrón State
#include "../core/EstadoPedido.h" 
// ==========================================

class Cliente;
class ItemPedido;
class Producto;
// Ya no es necesario 'class EstadoPedido;' porque el .h ya está incluido arriba

class Pedido
{
private:
    int id;
    std::weak_ptr<Cliente> cliente;
    std::vector<std::unique_ptr<ItemPedido>> items;
    
    // =================================================================
    // CAMBIO CLAVE 1: El estado ya no es un string, sino un puntero 
    // al objeto de estado (que es polimórfico).
    std::shared_ptr<EstadoPedido> estadoActual;
    // La variable anterior: // std::string estado;  <-- ELIMINADA
    // =================================================================

public:
    static const double IGV;

    // Constructor sigue igual, pero la implementación inicializa el estadoActual
    Pedido(int id, std::shared_ptr<Cliente> cliente); 
    ~Pedido();

    int getId() const;
    std::shared_ptr<Cliente> getCliente() const;
    const std::vector<std::unique_ptr<ItemPedido>> &getItems() const;
    
    // =================================================================
    // CAMBIO CLAVE 2: Eliminamos getEstado() y lo reemplazamos por el 
    // nombre del estado actual (delegando al objeto de estado).
    // Antes: // std::string getEstado() const; 
    std::string getEstadoNombre() const;
    // =================================================================

    // =================================================================
    // CAMBIO CLAVE 3: Los métodos de acción ahora DELEGAN en el objeto de estado.
    void avanzar(); 
    void cancelar(); 

    // MÉTODO AUXILIAR: Permite que las clases Estado (los estados concretos) 
    // cambien el estado (Contexto) del pedido.
    void setEstado(std::shared_ptr<EstadoPedido> nuevoEstado); 
    
    // El método setEstado(const std::string &nuevoEstado) ha sido eliminado.
    void agregarItem(std::shared_ptr<Producto> producto, int cantidad);
    double calcularTotal() const;
    void marcarComoPagado();

    friend std::ostream &operator<<(std::ostream &os, const Pedido &pedido)
    {
        // Sugerencia: Usar getEstadoNombre() para el debug si lo necesitas:
        // os << "Pedido ID: " << pedido.id << ", Estado: " << pedido.getEstadoNombre();
        os << "Pedido ID: " << pedido.id; 
        return os;
    }
};

#endif // PEDIDO_H