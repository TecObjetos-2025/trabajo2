#ifndef PEDIDO_H
#define PEDIDO_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

class Cliente;
class ItemPedido;
class Producto;

class Pedido
{
private:
    int id;
    std::weak_ptr<Cliente> cliente;
    std::vector<std::unique_ptr<ItemPedido>> items;
    std::string estado;

public:
    static const double IGV;

    Pedido(int id, std::shared_ptr<Cliente> cliente);
    ~Pedido();

    int getId() const;
    std::shared_ptr<Cliente> getCliente() const;
    const std::vector<std::unique_ptr<ItemPedido>> &getItems() const;
    std::string getEstado() const;

    void agregarItem(std::shared_ptr<Producto> producto, int cantidad);
    double calcularTotal() const;
    void setEstado(const std::string &nuevoEstado);
    void marcarComoPagado();

    friend std::ostream &operator<<(std::ostream &os, const Pedido &pedido)
    {
        // Simplificado para mostrar info basica
        os << "Pedido ID: " << pedido.id;

        // Para Debug mas adelante
        // os << "Pedido ID: " << pedido.id << ", Estado: " << pedido.estado << ", Total: $" << pedido.calcularTotal();
        return os;
    }
};

#endif // PEDIDO_H