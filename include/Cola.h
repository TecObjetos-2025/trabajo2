#ifndef COLA_H
#define COLA_H

#include <iostream>
#include <stdexcept>

// Nodo para la estructura de datos Cola
// Plantilla genérica para permitir cualquier tipo de dato
template <typename T>
struct Nodo
{
    T valor;
    Nodo<T> *siguiente;

    Nodo(T val) : valor(val), siguiente(nullptr) {}
};

// Plantilla genérica para la estructura de datos Cola
template <typename T>
class Cola
{
private:
    Nodo<T> *frente; // Puntero al inicio de la cola
    Nodo<T> *final;  // Puntero al final de la cola

public:
    // -- Metodos escenciales de la Cola --

    // Inicializar cola vacía
    Cola() : frente(nullptr), final(nullptr) {}

    ~Cola()
    {
        while (!estaVacia())
        {
            desencolar();
        }
    }

    bool estaVacia() const
    {
        return frente == nullptr;
    }

    // -- Metodos principales --
    /**
     * @brief Agregar un elemento al final de la cola
     *
     */
    void encolar(T valor)
    {
        // TODO: Implementar este metodo
    }

    /**
     * @brief Eliminar y devolver el elemento al frente de la cola
     */
    T desencolar()
    {
        // TODO: Implementar este metodo

        if (estaVacia())
        {
            throw std::runtime_error("Cola vacía. No se puede desencolar.");
        }
        T valor_temporal = frente->valor;
        return valor_temporal;
    }

    /**
     * @brief Devolver el elemento al frente de la cola sin eliminarlo
     */
    T obtenerFrente() const
    {
        // TODO: Implementar este metodo
        if (estaVacia())
        {
            throw std::runtime_error("Cola vacía. No se puede obtener el frente.");
        }
        return frente->valor;
    }
};

#endif // COLA_H