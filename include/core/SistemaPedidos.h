#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>

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
    std::vector<Pedido *> pedidos;
    std::vector<Observador *> observadores;
    std::vector<Producto *> productos;

    int proximoIdPersona = 1;

public:
    SistemaPedidos(); // Constructor

    ~SistemaPedidos(); // <- Nuevo para manejo de memoria al usar listas de punteros

    // Mejora de delegacion
    void mostrarMenu() const;

    void registrarPersona(Persona *persona);
    Persona *buscarPersonaPorId(int id);
    void mostrarTodasLasPersonas() const;

    // Gestion antigua en si
    // void registrarCliente(const std::string &nombre, const std::string &telefono);
    // Cliente *buscarClientePorId(int id);

    void inicializarMenu(); // Para agregar productos de ejemplo

    // Pedido &crearPedido(Cliente &cliente);
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);

    void agregarProducto(Producto *producto);
    void finalizarPedido(Pedido *pedido);

    // Métodos del patrón Observer
    void agregarObservador(Observador *obs);
    void notificarObservadores(const Pedido *pedido);
};

#endif // SISTEMAPEDIDOS_H