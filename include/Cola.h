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
        Nodo<T> *nuevoNodo = new Nodo<T>(valor);
        if (estaVacia())
        {
            frente = nuevoNodo;
            final = nuevoNodo;
        }
        else
        {
            final->siguiente = nuevoNodo;
            final = nuevoNodo;
        }
    }

    /**
     * @brief Eliminar y devolver el elemento al frente de la cola
     */
    T desencolar()
    {
        if (estaVacia())
        {
            throw std::runtime_error("Cola vacía. No se puede desencolar.");
        }
        // Copiar el valor del frente
        T valorARetornar = frente->valor;

        Nodo<T> *nodoAEliminar = frente;

        frente = frente->siguiente;

        if (frente == nullptr)
        {
            final = nullptr; // La cola está vacía después de desencolar
        }

        delete nodoAEliminar;

        return valorARetornar;
    }

    /**
     * @brief Mostrar todos los elementos de la cola (para depuración)
     */
    void mostrar() const
    {
        if (estaVacia())
        {
            std::cout << "Cola vacía." << std::endl;
            return;
        }

        // Puntero temporal para recorrer la cola
        Nodo<T> *actual = frente;
        std::cout << "Elementos en la cola (FRENTE): ";

        while (actual != nullptr)
        {
            std::cout << "[" actual->valor << "] -> ";
            actual = actual->siguiente;
        }
        std::cout << "FIN" << std::endl;
    }

    /**
     * @brief Devolver el elemento al frente de la cola sin eliminarlo
     */
    T obtenerFrente() const
    {
        if (estaVacia())
        {
            throw std::runtime_error("Cola vacía. No se puede obtener el frente.");
        }
        return frente->valor;
    }
};

#endif // COLA_H