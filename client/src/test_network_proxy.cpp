#include <QCoreApplication>
#include <QDebug>
#include "NetworkClientProxy.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    NetworkClientProxy proxy("127.0.0.1", 1234);

    try
    {
        auto menu = proxy.getMenu();
        qDebug() << "Menu size:" << (int)menu.size();
        for (const auto &p : menu)
        {
            qDebug() << p.id << QString::fromStdString(p.nombre) << p.precio;
        }

        // Crear pedido de prueba con el primer producto si existe
        if (!menu.empty())
        {
            std::vector<ItemPedidoCrear> items;
            ItemPedidoCrear it{menu.front().id, 1};
            items.push_back(it);
            proxy.finalizarPedido("ClienteTest", items, "nulo");
            qDebug() << "Pedido enviado correctamente.";
        }
    }
    catch (const std::exception &ex)
    {
        qWarning() << "Error:" << ex.what();
        return 1;
    }

    return 0;
}
