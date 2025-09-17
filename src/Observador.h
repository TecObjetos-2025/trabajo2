#ifndef OBSERVADOR_H
#define OBSERVADOR_H

// Buena practica cuando se usa punteros
class Pedido;

class Observador
{
public:
    virtual ~Observador() = default;                   // Destructor para herencia
    virtual void actualizar(const Pedido &pedido) = 0; // Método virtual puro (forzar implementacion)
};

#endif // OBSERVADOR_H