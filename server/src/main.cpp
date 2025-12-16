// main.cpp mínimo para ejecución en consola
#include <QCoreApplication>
#include "core/SistemaPedidos.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SistemaPedidos sistema;
    // Mantener la app viva
    return app.exec();
}
