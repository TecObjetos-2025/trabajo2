#ifndef PERSONA_H
#define PERSONA_H

#include <string>

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