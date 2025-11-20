#ifndef PEDIDO_H
#define PEDIDO_H

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
};

#endif // PEDIDO_H