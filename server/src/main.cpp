// main.cpp mínimo para ejecución en consola
#include <QCoreApplication>
#include "core/SistemaPedidos.h"
#include "repositories/ProductRepository.h"
#include "repositories/PedidoRepository.h"
#include "NetworkServer.h"
#include "db/DatabaseManager.h"
#include "repositories/ClienteRepository.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Crear repositorios y preparar la base de datos
    auto productRepository = std::make_shared<ProductRepository>();

    // Inicializar tablas y datos de seed (útil en ejecución local/CI)
    DatabaseManager::instance().inicializarTablas();
    if (!DatabaseManager::instance().seedData())
    {
        qWarning() << "Warning: seedData() no se ejecutó correctamente. La BD puede estar vacía.";
    }

    // Cargar datos desde BD
    productRepository->loadFromDB();

    auto pedidoRepository = std::make_shared<PedidoRepository>();
    auto clienteRepository = std::make_shared<repos::ClienteRepository>();

    // Inyectar repositorios en SistemaPedidos
    SistemaPedidos sistema(productRepository, pedidoRepository, clienteRepository);

    // Crear y arrancar servidor de red (usa el subsistema de pedidos)
    // El servidor se registra como observador para recibir eventos del sistema
    auto servidorPtr = std::make_shared<NetworkServer>(&sistema);
    sistema.registrarObservador(servidorPtr);
    // Nota: NetworkServer se mantiene en memoria a lo largo de la app

    // Mantener la app viva
    return app.exec();
}
