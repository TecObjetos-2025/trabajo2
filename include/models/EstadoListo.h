#ifndef ESTADOLISTO_H
#define ESTADOLISTO_H

#include "../core/EstadoPedido.h"

class EstadoListo : public EstadoPedido {
public:
    void avanzar(Pedido* pedido) override; // Ya no avanza más (o pasa a 'Entregado')
    void cancelar(Pedido* pedido) override; // No se puede cancelar
    std::string getNombre() const override { return "Listo"; }

    static std::shared_ptr<EstadoListo> getInstance();
private:
    EstadoListo() = default;
};

#endif // ESTADOLISTO_H