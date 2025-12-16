#ifndef EMPLEADO_H
#define EMPLEADO_H

#include "models/Persona.h"
#include <string>

/**
 * @brief Representa a un empleado de la cafetería, derivado de Persona.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class Empleado : public Persona
{
protected:
    std::string codigoEmpleado;

public:
    Empleado(int id, const std::string &nombre, const std::string &codigoEmpleado);

    void mostrarInfo() const override;
};

#endif // EMPLEADO_H