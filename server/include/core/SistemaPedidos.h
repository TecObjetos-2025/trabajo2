#ifndef SISTEMAPEDIDOS_H
#define SISTEMAPEDIDOS_H

#include <vector>
#include <string>
#include <memory>
#include "ColaThreadSafe.h"
#include "api/ICoreSistema.h"
#include "api/IObservadorCore.h"
#include <atomic>

class Persona;
class Pedido;
class Producto;
class Observador;
class MenuCafeteria;

class SistemaPedidos : public ICoreSistema
{
public:
    // Solo para pruebas: acceso read-only a productos
    const std::vector<std::shared_ptr<Producto>>& getProductos() const { return productos; }
    void cerrarColaPedidos() { pedidos_en_espera.cerrar(); }

private:
    /**
     * @brief Menú de la cafetería, contiene los productos disponibles.
     */
    std::unique_ptr<MenuCafeteria> menu;

    /**
     * @brief Personas registradas en el sistema (clientes, empleados, etc.).
     */
    std::vector<std::shared_ptr<Persona>> personas;

    /**
     * @brief Cola de pedidos en espera, gestionada de forma thread-safe.
     */
    ColaThreadSafe<std::shared_ptr<Pedido>> pedidos_en_espera;

    /**
     * @brief Observadores registrados para recibir notificaciones de cambios en pedidos.
     */
    std::vector<std::shared_ptr<IObservadorCore>> observadores;

    /**
     * @brief Productos registrados en el sistema.
     */
    std::vector<std::shared_ptr<Producto>> productos;

    /**
     * @brief Identificador incremental para registrar nuevas personas.
     */
    std::atomic<int> proximoIdPersona{1};

    /**
     * @brief Pedidos activos en el sistema.
     */
    std::vector<std::shared_ptr<Pedido>> listaMaestraPedidos;

public:
    SistemaPedidos();
    ~SistemaPedidos();

    // Delegación y gestión
    void mostrarMenu() const;
    /**
     * @brief Registra una nueva persona en el sistema.
     * @param persona Puntero compartido a la persona a registrar.
     */
    void registrarPersona(std::shared_ptr<Persona> persona);

    /**
     * @brief Busca una persona por su ID.
     * @param id Identificador único de la persona.
     * @return Persona* Puntero a la persona encontrada o nullptr si no existe.
     */
    Persona *buscarPersonaPorId(int id);

    /**
     * @brief Muestra todas las personas registradas en el sistema.
     */
    void mostrarTodasLasPersonas() const;

    /**
     * @brief Inicializa el menú de la cafetería con productos por defecto.
     */
    void inicializarMenu();

    /**
     * @brief Agrega un producto a un pedido existente.
     * @param pedido Referencia al pedido.
     * @param idProducto ID del producto a agregar.
     * @param cantidad Cantidad del producto.
     */
    void agregarProductoAPedido(Pedido &pedido, int idProducto, int cantidad);

    /**
     * @brief Agrega un producto al sistema.
     * @param producto Puntero compartido al producto.
     */
    void agregarProducto(std::shared_ptr<Producto> producto);

    /**
     * @brief Finaliza un pedido y lo notifica a los observadores.
     * @param pedido Puntero compartido al pedido a finalizar.
     */
    void finalizarPedido(std::shared_ptr<Pedido> pedido);

    // Métodos Observer API
    void registrarObservador(std::shared_ptr<IObservadorCore> observador);
    void removerObservador(IObservadorCore *observador) override;

    /**
     * @brief Notifica a todos los observadores registrados sobre el cambio de estado de un pedido.
     * @param pedido Puntero al pedido que ha cambiado de estado.
     */
    void notificarObservadores(const Pedido *pedido); // Interno

    // Métodos API
    std::vector<InfoProducto> getMenu() override;
    std::vector<InfoDescuento> getDescuentosDisponibles() override;

    double getPorcentajeIGV() const override;

    /**
     * @brief Finaliza un pedido a partir de los datos del cliente y los ítems, aplicando descuentos.
     * @param cliente Nombre o identificador del cliente.
     * @param items Lista de ítems a agregar al pedido.
     * @param id_descuentos Identificador de los descuentos a aplicar.
     */
    void finalizarPedido(const std::string &cliente,
                         const std::vector<ItemPedidoCrear> &items,
                         const std::string &id_descuentos) override;

    std::vector<InfoPedido> getPedidosActivos() override;

    /**
     * @brief Procesa el siguiente pedido en la cola de espera y lo notifica a los observadores.
     */
    void procesarSiguientePedido() override;

    // Otros
    std::shared_ptr<Pedido> procesarSiguientePedidoInterno();
    void notificarPedidoTerminado(const std::shared_ptr<Pedido> &pedido);
    /**
     * @brief Muestra los pedidos que están actualmente en espera en la cola.
     */
    void mostrarPedidosEnEspera() const;
};

#endif // SISTEMAPEDIDOS_H