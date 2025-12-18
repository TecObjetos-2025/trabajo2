// main.cpp mínimo para ejecución en consola
#include <QCoreApplication>
#include "core/SistemaPedidos.h"
#include "repositories/ProductRepository.h"
#include "repositories/PedidoRepository.h"
#include "NetworkServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Crear repositorios y cargar datos
    auto productRepository = std::make_shared<ProductRepository>();
    productRepository->loadFromDB();

    auto pedidoRepository = std::make_shared<PedidoRepository>();

    // Inyectar repositorios en SistemaPedidos
    SistemaPedidos sistema(productRepository, pedidoRepository);

    // Crear y arrancar servidor de red (usa el subsistema de pedidos)
    NetworkServer servidor(&sistema);

    // Mantener la app viva
    return app.exec();
}
