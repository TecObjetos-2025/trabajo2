#include "CoreFalso.h"

CoreFalso::CoreFalso()
{
    std::cout << "[CoreFalso] Inicializado." << std::endl;
}

// Gestion de Observadores
void CoreFalso::registrarObservador(IObservadorCore *observador)
{
    observadores.push_back(observador);
    std::cout << "[CoreFalso] Observador registrado." << std::endl;
}

void CoreFalso::removerObservador(IObservadorCore *observador)
{
    // Simplificación: No se implementa la remoción real en este core falso.
    // observadores.erase(std::remove(observadores.begin(), observadores.end(), observador), observadores.end());
    std::cout << "[CoreFalso] Observador removido." << std::endl;
}

// Funciones "Vista Cajero"
std::vector<InfoProducto> CoreFalso::getMenu()
{
    std::cout << "[CoreFalso] getMenu llamado. Devolviendo 3 productos fake" << std::endl;
    return {
        {101, "Cafe Americano", 5.50},
        {102, "Pan con Chicharron", 7.00},
        {103, "Jugo de Naranja", 6.00}};
};

std::vector<InfoDescuento> CoreFalso::getDescuentosDisponibles()
{
    std::cout << "[CoreFalso] getDescuentosDisponibles llamado. Devolviendo 2 descuentos fake" << std::endl;
    return {
        {"NINGUNO", "Sin descueto"},
        {"2X1_BEBIDAS", "2x1 en Bebidas"},
        {"DESCUENTO_MATUTINO", "10% de descuento en pedidos antes de las 10 AM"}};
};

void CoreFalso::finalizarPedido(const std::string &cliente,
                                const std::vector<ItemPedidoCrear> &items,
                                const std::string &id_descuentos)
{
    std::cout << "[CoreFalso] finalizarPedido llamado para cliente: " << cliente << std::endl;
    std::cout << " Items: " << items.size() << std::endl;
    std::cout << " Descuento: " << id_descuentos << std::endl;

    // TODO: Notificar a los observadores que hay nuevos pedidos en cola
    // for (auto obs : observadores)
    // {
    //    obs->onNuevosPedidosEnCola();
    // }
}

// Funciones "Vista Cocina"
std::vector<InfoPedido> CoreFalso::getPedidosEnCola()
{
    std::cout << "[CoreFalso] getPedidosEnCola llamado. Devolviendo 2 pedidos fake" << std::endl;

    // Datos fake
    InfoPedido pedido1;
    pedido1.id_pedido = 1001;
    pedido1.cliente = "Cliente Falso 1";
    pedido1.estado = "En Cola";
    pedido1.total_final = 11.00;
    pedido1.items = {
        {"Cafe Americano", 2, 5.50}};

    InfoPedido pedido2;
    pedido2.id_pedido = 1002;
    pedido2.cliente = "Cliente Falso 2";
    pedido2.estado = "En Cola";
    pedido2.total_final = 13.00;
    pedido2.items = {
        {"Pan con Chicharron", 1, 7.00},
        {"Jugo de Naranja", 1, 6.00}};

    return {pedido1, pedido2};
}

void CoreFalso::procesarSiguientePedido()
{
    std::cout << "[CoreFalso] procesarSiguientePedido llamado. Simulando procesamiento..." << std::endl;

    // TODO: Notificar a los observadores que un pedido ha sido terminado
    // for (auto obs : observadores)
    // {
    //    obs->onPedidoTerminado(1001); // ID de pedido fake
    // }
}
