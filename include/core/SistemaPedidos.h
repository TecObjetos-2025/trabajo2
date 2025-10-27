#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>
#include "Cola.h"

class Persona;
class Pedido;
class Producto;
class Observador;
class MenuCafeteria;

class SistemaPedidos
{
private:
    MenuCafeteria *menu;
    std::vector<Persona *> personas;
    // std::vector<Pedido *> pedidos;
    Cola<Pedido *> pedidos_en_espera; // <- Cambio a Cola de punteros (FIFO)
    std::vector<Observador *> observadores;
    std::vector<Producto *> productos;

    int proximoIdPersona = 1;

public:
    SistemaPedidos(); // Constructor

    ~SistemaPedidos(); // <- Manejo de memoria en el destructor

    // Mejora de delegacion
    void mostrarMenu() const;

    void registrarPersona(Persona *persona);
    Persona *buscarPersonaPorId(int id);
    void mostrarTodasLasPersonas() const;

    void inicializarMenu(); // Para agregar productos de ejemplo

    // Pedido &crearPedido(Cliente &cliente);
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);

    void agregarProducto(Producto *producto);

    /**
     * @brief Actuar como "Procesador de Pedidos": agregar un nuevo pedido a la cola
     */
    void finalizarPedido(Pedido *pedido);

    // Métodos del patrón Observer
    void agregarObservador(Observador *obs);
    void notificarObservadores(const Pedido *pedido);

    /**
     * @brief Metodo para actuar como "Consumidor de Pedidos": procesar el siguiente pedido en la cola
     */
    Pedido *procesarSiguientePedido(); // <- NUEVO

    void mostrarPedidosEnEspera() const; // <- NUEVO
};

#endif // SISTEMAPEDIDOS_H