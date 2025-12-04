#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "core/SistemaPedidos.h"
#include "core/CafeteriaFactory.h"

#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "[MainWindow] Inicializando componentes backend...";

    // Inicializar el core real
    auto sistema = std::make_shared<SistemaPedidos>();
    sistema->inicializarMenu(); // Asegurar que el menu tenga datos

    // Registrar un cliente para pruebas
    auto clienteDummy = CafeteriaFactory::crearCliente(1, "Cliente Prueba", "999-888-777");
    sistema->registrarPersona(clienteDummy);

    coreSistema = sistema;

    coreAdapter = std::make_shared<CoreQtAdapter>();

    // Registrar observador
    coreSistema->registrarObservador(coreAdapter);

    // Conectar señales y slots
    conectarSenalesYSlots();

    // Cargar datos iniciales en la UI
    cargarMenuEnUI();
    cargarPedidosEnUI();

    // Descuentos al ComboBox
    ui->comboDescuentos->clear();
    auto descuentos = coreSistema->getDescuentosDisponibles();
    for (const auto &desc : descuentos)
    {
        ui->comboDescuentos->addItem(QString::fromStdString(desc.descripcion),
                                     QString::fromStdString(desc.id_descuento));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Conecta las señales y slots necesarios entre la UI y el core.
 */
void MainWindow::conectarSenalesYSlots()
{
    // --- Conectar señales del CoreQtAdapter a los slots de MainWindow

    // Conectar señal de nuevos pedidos en cola
    connect(coreAdapter.get(), &CoreQtAdapter::nuevosPedidosEnCola,
            this, &MainWindow::onPedidosActualizados);

    // Conectar señal de error
    connect(coreAdapter.get(), &CoreQtAdapter::errorOcurrido,
            this, &MainWindow::onCoreError);

    // NOTA: Los botones ya están conectados automáticamente por Qt a los slots
}

// --- Funciones para cargar datos en la UI
void MainWindow::cargarMenuEnUI()
{
    ui->listaMenuProductos->clear();

    auto menu = coreSistema->getMenu();
    for (const auto &producto : menu)
    {
        QString itemText = QString("%1 - %2 (S/%3)")
                               .arg(producto.id)
                               .arg(QString::fromStdString(producto.nombre))
                               .arg(producto.precio, 0, 'f', 2);
        QListWidgetItem *item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, producto.id); // Almacenar el ID del producto
        ui->listaMenuProductos->addItem(item);
    }
}

void MainWindow::cargarPedidosEnUI()
{
    ui->listaColaPedidos->clear();
    auto pedidos = coreSistema->getPedidosEnCola();

    for (const auto &pedido : pedidos)
    {
        QString itemText = QString("ID: %1 | %2 | %3")
                               .arg(pedido.id_pedido)
                               .arg(QString::fromStdString(pedido.cliente))
                               .arg(QString::fromStdString(pedido.estado));
        QListWidgetItem *item = new QListWidgetItem(itemText);
        // Se podria almacenar más data si es necesario
        ui->listaColaPedidos->addItem(item);
    }
}

void MainWindow::actualizarTotalesUI()
{
    double subtotal = 0.0;

    for (int i = 0; i < ui->listaOrdenActual->count(); ++i)
    {
        QListWidgetItem *item = ui->listaOrdenActual->item(i);
        double precio = item->data(RolePrecioUnitario).toDouble(); // Recuperar precio
        subtotal += precio;
    }

    double tasaIGV = coreSistema->getPorcentajeIGV();

    double montoIGV = subtotal * tasaIGV;
    double total = subtotal + montoIGV;

    // Actualizacion visual
    ui->lblSubtotal->setText(QString("S/ %1").arg(subtotal, 0, 'f', 2));
    ui->lblIGV->setText(QString("S/ %1").arg(montoIGV, 0, 'f', 2));

    // Total en negrita
    ui->lblTotal->setText(QString("S/ %1").arg(total, 0, 'f', 2));
}

// --- Slots de la UI

// Vista Cajero
void MainWindow::on_btnAnadirItem_clicked()
{
    // Obtener item seleccionado
    QListWidgetItem *itemSeleccionado = ui->listaMenuProductos->currentItem();
    if (!itemSeleccionado)
    {
        onCoreError("Por favor, seleccione un producto del menú.");
        return;
    }

    int id = itemSeleccionado->data(Qt::UserRole).toInt();

    // Recuperar precio, forma sucia (puede mejorarse)
    QString texto = itemSeleccionado->text();
    int posMoneda = texto.lastIndexOf("S/");
    int posFin = texto.lastIndexOf(")");
    // qDebug() << "Posicion de S/: " << posMoneda;
    // qDebug() << "Texto a partir de Posicion " << texto.mid(posMoneda);
    double precio = texto.mid(posMoneda + 2, posFin - posMoneda - 2).toDouble();

    qDebug() << "[MainWindow] Añadiendo ítem al pedido actual: ID =" << id << ", Precio =" << precio;

    // Crear el item visual
    QListWidgetItem *newItem = new QListWidgetItem(itemSeleccionado->text());
    newItem->setData(Qt::UserRole, id);
    newItem->setData(Qt::UserRole + 1, precio); // Guardar precio

    // Anadir al pedido actual
    ui->listaOrdenActual->addItem(newItem);
    qDebug() << "[MainWindow] Ítem añadido al pedido actual.";
    actualizarTotalesUI();
}

void MainWindow::on_btnQuitarItem_clicked()
{
    // Obtener la fila actual
    int fila = ui->listaOrdenActual->currentRow();

    if (fila >= 0)
    {
        // 'takeItem' elimina el ítem de la lista pero no borra la memoria
        QListWidgetItem *item = ui->listaOrdenActual->takeItem(fila);
        delete item; // Liberar memoria manual

        // Recalcular
        actualizarTotalesUI();
    }
}

void MainWindow::on_btnFinalizarPedido_clicked()
{
    qDebug() << "[MainWindow] Finalizando pedido...";

    // Recopilar datos del pedido
    std::string cliente = ui->lineEditCliente->text().toStdString();
    if (cliente.empty())
    {
        onCoreError("Por favor, ingrese el nombre del cliente.");
        return;
    }

    std::string id_descuento = ui->comboDescuentos->currentData().toString().toStdString();

    std::vector<ItemPedidoCrear> items;
    for (int i = 0; i < ui->listaOrdenActual->count(); ++i)
    {
        QListWidgetItem *item = ui->listaOrdenActual->item(i);
        int productoId = item->data(Qt::UserRole).toInt();
        items.push_back({productoId, 1}); // Cantidad fija de 1 por simplicidad
    }

    if (items.empty())
    {
        onCoreError("El pedido no puede estar vacío.");
        return;
    }

    // Llamar al 'API' del core
    coreSistema->finalizarPedido(cliente, items, id_descuento);

    // Limpiar UI
    ui->lineEditCliente->clear();
    ui->listaOrdenActual->clear();
    ui->comboDescuentos->setCurrentIndex(0);

    QMessageBox::information(this, "Pedido Creado", "El pedido ha sido creado y enviado a la cocina.");

    // NOTA: La actualización de la cola de pedidos se hará vía señal del core
}

// Vista Cocina
void MainWindow::on_btnProcesarSiguientePedido_clicked()
{
    qDebug() << "[MainWindow] Procesando siguiente pedido...";

    try
    {
        coreSistema->procesarSiguientePedido();
        QMessageBox::information(this, "Pedido Procesado", "El siguiente pedido ha sido procesado.");
    }
    catch (const std::exception &e)
    {
        onCoreError(e.what());
    }
}

// --- Slots para manejar señales del CoreQtAdapter
void MainWindow::onPedidosActualizados()
{
    qDebug() << "[MainWindow] Slot 'onPedidosActualizados' activado.";
    cargarPedidosEnUI();
}

void MainWindow::onCoreError(QString mensaje)
{
    qDebug() << "[MainWindow] Slot 'onCoreError' activado con mensaje:" << mensaje;
    QMessageBox::critical(this, "Error del Sistema", mensaje);
}
