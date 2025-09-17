#ifndef PEDIDO_H
#define PEDIDO_H

#include <vector>
#include "ItemPedido.h"

// Buena practica cuando se usa punteros
class Cliente;

class Pedido
{
private:
    int id;
    const Cliente &cliente; // Asociado a un cliente
    std::vector<ItemPedido> items;
    std::string estado; // Por ejemplo: "EN_PROCESO", "COMPLETADO"

public:
    Pedido(int id, const Cliente &cliente);

    int getId() const;
    const Cliente &getCliente() const;
    const std::vector<ItemPedido> &getItems() const;
    std::string getEstado() const;

    void agregarItem(const Producto &producto, int cantidad);
    double calcularTotal() const;
    void setEstado(const std::string &nuevoEstado);
};

#endif // PEDIDO_H