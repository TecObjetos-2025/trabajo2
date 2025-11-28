#include <thread>
#include <atomic>
#include <chrono>
#include "models/Cocinero.h"
#include "models/Pedido.h"
#include "core/SistemaPedidos.h"
#include <iostream>

Cocinero::Cocinero(int id, const std::string &nombre, const std::string &codigoEmpleado, SistemaPedidos *sistema)
    : Empleado(id, nombre, codigoEmpleado), sistema(sistema) {}

void Cocinero::iniciar()
{
    activo = true;
    hiloCocina = std::thread(&Cocinero::cicloCocina, this);
}

void Cocinero::detener()
{
    activo = false;
    if (hiloCocina.joinable())
        hiloCocina.join();
}

void Cocinero::cicloCocina()
{
    try
    {
        while (activo)
        {
            // Consumir pedidos de la cola
            auto pedido = this->sistema->procesarSiguientePedidoInterno();
            if (pedido)
            {
                std::cout << "[COCINERO] " << this->getNombre() << " cocinando Pedido #"
                          << pedido->getId() << "..." << std::endl;
                // this->sistema->notificarObservadores();

                // De estado "EN_COLA" a "EN_PREPARACION"
                pedido->avanzar();

                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simula tiempo de cocina

                // DE estado "EN_PREPARACION" a "LISTO"
                pedido->avanzar();

                std::cout << "[COCINERO] Pedido #" << pedido->getId() << " listo!" << std::endl;
                // Notificar al sistema que el pedido ha sido terminado
                sistema->notificarPedidoTerminado(pedido);
            }
            else
            {
                // Si no hay pedidos, dormir un poco antes de volver a intentar
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "[COCINERO] Hilo finalizado: " << e.what() << std::endl;
    }
}

void Cocinero::mostrarInfo() const
{
    std::cout << "--- Ficha de Cocinero ---" << std::endl;
    std::cout << "ID: " << id << ", Nombre: " << nombre
              << ", Puesto: Cocinero, Codigo: " << codigoEmpleado << std::endl;
}

void Cocinero::actualizar(const Pedido *pedido)
{
    // Util para la UI mas adelante
    std::cout << "\n================ AVISO PARA COCINA ================" << std::endl;
    std::cout << "¡Nuevo pedido #" << pedido->getId() << " para preparar!" << std::endl;
    std::cout << "===================================================" << std::endl;
}

// Nueva implementacion
void Cocinero::cocinarSiguientePedido()
{
    // Cocinero como "Consumidor de Pedidos"
    auto pedido = this->sistema->procesarSiguientePedidoInterno();

    // Responsabilidad de procesar el pedido
    if (pedido)
    {
        std::cout << "[COCINERO] " << this->getNombre() << " cocinando Pedido #"
                  << pedido->getId() << "..." << std::endl;
        // Simular tiempo de cocina
        // std::this_thread::sleep_for(std::chrono::seconds(2));

        pedido->avanzar(); // En Cola -> En Preparación
        pedido->avanzar(); // En Preparación -> Listo
        // ======================================

        std::cout << "[COCINERO] Pedido #" << pedido->getId() << " listo!" << std::endl;
        sistema->notificarPedidoTerminado(pedido);
    }
}

// Métodos de IObservadorCore (stubs)
void Cocinero::onNuevosPedidosEnCola()
{
    std::cout << "[Cocinero] Notificado: Hay nuevos pedidos en cola." << std::endl;
}

void Cocinero::onPedidoTerminado(int id_pedido)
{
    std::cout << "[Cocinero] Notificado: Pedido terminado, id=" << id_pedido << std::endl;
}

void Cocinero::onError(const std::string &mensaje)
{
    std::cout << "[Cocinero] Error: " << mensaje << std::endl;
}
