#ifndef COCINERO_H
#define COCINERO_H

#include "models/Empleado.h"
#include "patterns/Observador.h"

// Doble herencia segun diseño previo Cocinero es un Empleado y también es un Observador
class Cocinero : public Empleado, public Observador
{
public:
    Cocinero(int id, const std::string &nombre, const std::string &codigoEmpleado);

    void mostrarInfo() const override;

    void actualizar(const Pedido *pedido) override;
};

#endif // COCINERO_H