#ifndef IESTADOPEDIDO_H
#define IESTADOPEDIDO_H

#include <string>
#include <memory>

// Declaración anticipada del Contexto (Pedido)
class Pedido;

/**
 * @brief Interfaz abstracta para el Patrón State.
 * Define los métodos que los estados concretos deben implementar.
 */
class IEstadoPedido
{
public:
    // Destructor virtual: Es OBLIGATORIO para el polimorfismo
    // y para evitar fugas de memoria al usar shared_ptr con clases derivadas.
    virtual ~IEstadoPedido() = default;

    // Métodos virtuales puros (deben ser implementados por los estados concretos)
    virtual void avanzar(Pedido *pedido) = 0;
    virtual void cancelar(Pedido *pedido) = 0;

    // Método para obtener el nombre del estado (para logs o UI)
    virtual std::string getNombre() const = 0;
};

#endif // IESTADOPEDIDO_H