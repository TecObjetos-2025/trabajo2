#include "Cocina.h"
#include "Pedido.h"
#include "Cliente.h"

void Cocina::actualizar(const Pedido &pedido)
{
    // Solo imprimir en pantalla, quiza en el futuro
    // puede ser algo mas chevere
    std::cout << "\n================ AVISO PARA COCINA ================" << std::endl;
    std::cout << "Nuevo pedido recibido! ID del Pedido: " << pedido.getId() << std::endl;
    std::cout << "Cliente: " << pedido.getCliente().getNombre() << std::endl;
    std::cout << "Items a preparar:" << std::endl;

    // Recorrer los items del pedido (nombre y cantidad)
    for (const auto &item : pedido.getItems())
    {
        std::cout << "  - " << item.getCantidad() << "x " << item.getProducto().getNombre() << std::endl;
    }
    std::cout << "===================================================" << std::endl;
}