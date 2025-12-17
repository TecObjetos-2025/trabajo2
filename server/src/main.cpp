// main.cpp mínimo para ejecución en consola
#include <QCoreApplication>
#include "core/SistemaPedidos.h"
#include "repositories/ProductRepository.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Crear repositorio y cargar datos
    auto productRepository = std::make_shared<ProductRepository>();
    productRepository->loadFromDB();

    // Inyectar repositorio en SistemaPedidos
    SistemaPedidos sistema(productRepository);

    // Mantener la app viva
    return app.exec();
}
