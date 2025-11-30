#ifndef COCINERO_H
#define COCINERO_H

#include <thread>
#include "models/Empleado.h"
#include "patterns/Observador.h"
#include "api/IObservadorCore.h"
#include <atomic>

class SistemaPedidos; // Declaración adelantada

/**
 * @brief Representa al cocinero de la cafetería, encargado de preparar pedidos y recibir notificaciones.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 */
class Cocinero : public Empleado, public Observador, public IObservadorCore
{
private:
    SistemaPedidos *sistema; // <- Puntero al sistema de pedidos
    std::thread hiloCocina;
    std::atomic<bool> activo{false};

    // Ciclo principal del hilo consumidor
    void cicloCocina();

public:
    Cocinero(int id, const std::string &nombre, const std::string &codigoEmpleado, SistemaPedidos *sistema = nullptr);

    void mostrarInfo() const override;
    void actualizar(const Pedido *pedido) override;
    void cocinarSiguientePedido();

    // Multithreading
    void iniciar();
    void detener();

    // Métodos de IObservadorCore
    void onNuevosPedidosEnCola() override;
    void onPedidoTerminado(int id_pedido) override;
    void onError(const std::string &mensaje) override;
};

#endif // COCINERO_H