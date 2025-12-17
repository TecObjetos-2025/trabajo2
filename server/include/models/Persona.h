#ifndef PERSONA_H
#define PERSONA_H

#include <string>

/**
 * @brief Clase base abstracta para personas en la cafetería (clientes, empleados, etc.).
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class Persona
{
protected:
    int id;
    std::string nombre;

public:
    Persona(int id, const std::string &nombre);
    virtual ~Persona() = default;

    std::string getNombre() const;
    int getId() const;

    // Para clases abstracta
    virtual void mostrarInfo() const = 0;
};

#endif // PERSONA_H