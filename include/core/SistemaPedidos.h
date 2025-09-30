#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "MenuCafeteria.h"
#include "patterns/Observador.h"

class SistemaPedidos
{
private:
    MenuCafeteria menu;
    std::vector<Cliente> clientes;
    std::vector<Pedido> pedidos;
    std::vector<Observador *> observadores;
    int proximoIdCliente = 1;
    int proximoIdPedido = 1;

public:
    SistemaPedidos(); // Constructor

    // Mejora de delegacion
    void mostrarMenu() const;

    // Gestion en si
    void registrarCliente(const std::string &nombre, const std::string &telefono);
    Cliente *buscarClientePorId(int id);

    void inicializarMenu(); // Para agregar productos de ejemplo

    Pedido &crearPedido(Cliente &cliente);
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);

    void finalizarPedido(Pedido &pedido);

    // Métodos del patrón Observer
    void agregarObservador(Observador *obs);
    void notificarObservadores(const Pedido &pedido);
};

#endif // SISTEMAPEDIDOS_H