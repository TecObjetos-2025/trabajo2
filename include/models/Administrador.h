#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "models/Empleado.h"
#include "patterns/Observador.h"
#include "api/IObservadorCore.h"
#include <map>
#include <string>

/**
 * @brief Representa al administrador de la cafetería, encargado de gestionar ventas y notificaciones.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class Administrador : public Empleado, public Observador, public IObservadorCore
{
private:
    // Map para estadisticas <Producto, cantidadVendida>
    std::map<std::string, int> ventasPorProducto;

public:
    Administrador(int id, const std::string &nombre, const std::string &codigoEmpleado);

    void mostrarInfo() const override;
    void actualizar(const Pedido *pedido) override;
    void mostrarEstadisticasVentas() const;

    // Métodos de IObservadorCore
    void onNuevosPedidosEnCola() override;
    void onPedidoTerminado(int id_pedido) override;
    void onError(const std::string &mensaje) override;
};

#endif // ADMINISTRADOR_H