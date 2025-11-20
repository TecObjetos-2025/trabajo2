#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>
#include <memory>
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
    std::unique_ptr<MenuCafeteria> menu;
    std::vector<std::shared_ptr<Persona>> personas;
    Cola<std::shared_ptr<Pedido>> pedidos_en_espera;
    std::vector<std::shared_ptr<IObservadorCore>> observadores;
    std::vector<std::shared_ptr<Producto>> productos;

    int proximoIdPersona = 1;

public:
    // Satisface la interfaz ICoreSistema
    void registrarObservador(IObservadorCore *observador) override;
    SistemaPedidos();
    ~SistemaPedidos();

    // Delegación y gestión
    void mostrarMenu() const;
    void registrarPersona(std::shared_ptr<Persona> persona);
    Persona *buscarPersonaPorId(int id);
    void mostrarTodasLasPersonas() const;
    void inicializarMenu();
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);
    void agregarProducto(std::shared_ptr<Producto> producto);
    void finalizarPedido(std::shared_ptr<Pedido> pedido);

    // Métodos Observer API
    void registrarObservador(std::shared_ptr<IObservadorCore> observador);
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