#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "models/Empleado.h"
#include "patterns/Observador.h"
#include <map>
#include <string>

class Administrador : public Empleado, public Observador
{
private:
    // Map para estadisticas <Producto, cantidadVendida>
    std::map<std::string, int> ventasPorProducto;

public:
    Administrador(int id, const std::string &nombre, const std::string &codigoEmpleado);

    void mostrarInfo() const override;
    void actualizar(const Pedido *pedido) override;
    void mostrarEstadisticasVentas() const;
};

#endif // ADMINISTRADOR_H