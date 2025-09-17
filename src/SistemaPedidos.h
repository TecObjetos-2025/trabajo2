#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>
#include "Pedido.h"
#include "Cliente.h"
#include "MenuCafeteria.h"
#include "Observador.h"

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

    // Gestion en si
    void registrarCliente(const std::string &nombre, const std::string &telefono);
    Cliente *buscarClientePorId(int id);

    void inicializarMenu(); // Para agregar productos de ejemplo

    Pedido &crearPedido(Cliente &cliente);
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);

    // Métodos del patrón Observer
    void agregarObservador(Observador *obs);
    void notificarObservadores(const Pedido &pedido);
};

#endif // SISTEMAPEDIDOS_H