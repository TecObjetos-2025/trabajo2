#ifndef COCINA_H
#define COCINA_H

#include "Observador.h"
#include <iostream>

class Cocina : public Observador
{
public:
    void actualizar(const Pedido &pedido) override;
};

#endif // COCINA_H