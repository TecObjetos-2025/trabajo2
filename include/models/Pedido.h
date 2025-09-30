#ifndef PEDIDO_H
#define PEDIDO_H

#include <vector>
#include <string>

class Cliente;
class ItemPedido;
class Producto;

class Pedido
{
private:
    int id;
    const Cliente *cliente;          // Asociado a un cliente
    std::vector<ItemPedido *> items; // <- Nuevo: Vector de punteros
    std::string estado;              // Por ejemplo: "EN_PROCESO", "COMPLETADO"

public:
    static const double IGV = 0.18; // Variable de clase

    Pedido(int id, const Cliente *cliente);
    ~Pedido(); // <- Nuevo para liberar memoria

    int getId() const;
    const Cliente *getCliente() const; // Devuelve el puntero ahora
    const std::vector<ItemPedido *> &getItems() const;
    std::string getEstado() const;

    void agregarItem(const Producto *producto, int cantidad);
    double calcularTotal() const;
    void setEstado(const std::string &nuevoEstado);
    void marcarComoPagado();
};

#endif // PEDIDO_H