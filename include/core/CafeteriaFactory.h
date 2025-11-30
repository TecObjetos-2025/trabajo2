#ifndef CAFETERIAFACTORY_H
#define CAFETERIAFACTORY_H

#include <memory>
#include <string>
#include "models/Pedido.h"
#include "models/Cliente.h"
#include "models/Cocinero.h"
#include "models/Administrador.h"

class SistemaPedidos;

class CafeteriaFactory {
public:
    static std::shared_ptr<Cliente> crearCliente(int id, const std::string& nombre, const std::string& telefono);
    static std::shared_ptr<Cocinero> crearCocinero(int id, const std::string& nombre, const std::string& codigo, SistemaPedidos* sistema);
    static std::shared_ptr<Administrador> crearAdministrador(int id, const std::string& nombre, const std::string& codigo);
    static std::shared_ptr<Pedido> crearPedido(int id, std::shared_ptr<Cliente> cliente);
};

#endif