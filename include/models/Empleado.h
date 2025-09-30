#ifndef EMPLEADO_H
#define EMPLEADO_H

#include "models/Persona.h"
#include <string>

class Empleado : public Persona
{
protected:
    std::string codigoEmpleado;

public:
    Empleado(int id, const std::string &nombre, const std::string &codigoEmpleado);

    void mostrarInfo() const override;
};

#endif // EMPLEADO_H