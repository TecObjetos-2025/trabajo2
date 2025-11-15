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

#include "api/ICoreSistema.h"
#include "api/IObservadorCore.h"

class SistemaPedidos : public ICoreSistema
{
private:
    MenuCafeteria *menu;
    std::vector<Persona *> personas;
    Cola<Pedido *> pedidos_en_espera;
    std::vector<IObservadorCore *> observadores; // Refactor: ahora son IObservadorCore*
    std::vector<Producto *> productos;

    int proximoIdPersona = 1;

public:
    SistemaPedidos();
    ~SistemaPedidos();

    // Delegación y gestión
    void mostrarMenu() const;
    void registrarPersona(Persona *persona);
    Persona *buscarPersonaPorId(int id);
    void mostrarTodasLasPersonas() const;
    void inicializarMenu();
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);
    void agregarProducto(Producto *producto);
    void finalizarPedido(Pedido *pedido);

    // Métodos Observer API
    void registrarObservador(IObservadorCore *observador) override;
    void removerObservador(IObservadorCore *observador) override;
    void notificarObservadores(const Pedido *pedido); // Interno

    // Métodos API
    std::vector<InfoProducto> getMenu() override;
    std::vector<InfoDescuento> getDescuentosDisponibles() override;
    void finalizarPedido(const std::string &cliente,
                         const std::vector<ItemPedidoCrear> &items,
                         const std::string &id_descuentos) override;
    std::vector<InfoPedido> getPedidosEnCola() override;
    void procesarSiguientePedido() override;

    // Otros
    Pedido *procesarSiguientePedidoInterno();
    void mostrarPedidosEnEspera() const;
};

#endif // SISTEMAPEDIDOS_H