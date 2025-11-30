#include "core/CafeteriaFactory.h"
#include "core/SistemaPedidos.h"

std::shared_ptr<Cliente> CafeteriaFactory::crearCliente(int id, const std::string& nombre, const std::string& telefono) {
    return std::make_shared<Cliente>(id, nombre, telefono);
}

std::shared_ptr<Cocinero> CafeteriaFactory::crearCocinero(int id, const std::string& nombre, const std::string& codigo, SistemaPedidos* sistema) {
    return std::make_shared<Cocinero>(id, nombre, codigo, sistema);
}

std::shared_ptr<Administrador> CafeteriaFactory::crearAdministrador(int id, const std::string& nombre, const std::string& codigo) {
    return std::make_shared<Administrador>(id, nombre, codigo);
}

std::shared_ptr<Pedido> CafeteriaFactory::crearPedido(int id, std::shared_ptr<Cliente> cliente) {
    return std::make_shared<Pedido>(id, cliente);
}