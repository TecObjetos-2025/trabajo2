#ifndef COCINERO_H
#define COCINERO_H

#include "models/Empleado.h"
#include "patterns/Observador.h"

class SistemaPedidos; // Declaración adelantada

// Doble herencia segun diseño previo Cocinero es un Empleado y también es un Observador
class Cocinero : public Empleado, public Observador
{
private:
    SistemaPedidos *sistema; // <- Puntero al sistema de pedidos
public:
    Cocinero(int id, const std::string &nombre, const std::string &codigoEmpleado, SistemaPedidos *sistema = nullptr);

    void mostrarInfo() const override;

    void actualizar(const Pedido *pedido) override;

    /**
     * @brief Cocinero asume el rol de "Consumidor de Pedidos": procesa el siguiente pedido en la cola
     */
    void cocinarSiguientePedido();
};

#endif // COCINERO_H