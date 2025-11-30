#ifndef CLIENTE_H
#define CLIENTE_H

#include "models/Persona.h"
#include <string>

/**
 * @brief Representa un cliente de la cafetería, derivado de Persona.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class Cliente : public Persona
{
private:
    std::string telefono;

public:
    Cliente(int id, const std::string &nombre, const std::string &telefono);

    std::string getTelefono() const;

    void mostrarInfo() const override;
};

#endif // CLIENTE_H