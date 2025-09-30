#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente
{
private:
    int id;
    std::string nombre;
    std::string telefono;

public:
    Cliente(int id, const std::string &nombre, const std::string &telefono);

    int getId() const;
    std::string getNombre() const;
};

#endif // CLIENTE_H