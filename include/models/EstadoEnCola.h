#ifndef ESTADOENCOLA_H
#define ESTADOENCOLA_H

#include "../core/EstadoPedido.h"

class EstadoEnCola : public EstadoPedido {
public:
    void avanzar(Pedido* pedido) override;
    void cancelar(Pedido* pedido) override;
    std::string getNombre() const override { return "En Cola"; }

    // Proporciona acceso a la única instancia de este estado (Singleton)
    static std::shared_ptr<EstadoEnCola> getInstance();
private:
    EstadoEnCola() = default; // Constructor privado
};

#endif // ESTADOENCOLA_H