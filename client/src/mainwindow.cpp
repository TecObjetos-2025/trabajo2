#include "mainwindow.h"
#include "./ui_mainwindow.h"
// #include "core/SistemaPedidos.h"
// #include "core/CafeteriaFactory.h"

#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include "../../common/include/api/Protocolo.h"

MainWindow::MainWindow(ICoreSistema *core, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_core(core)
{
    ui->setupUi(this);

    qDebug() << "[MainWindow] Inicializando componentes backend...";

    // wrappear el puntero crudo en shared_ptr sin ownership
    if (m_core)
    {
        coreSistema = std::shared_ptr<ICoreSistema>(m_core, [](ICoreSistema *)
                                                    {
            /* no-op deleter: la vida la gestiona quien creó el proxy */ });
    }
    else
    {
        coreSistema = nullptr;
    }

    coreAdapter = std::make_shared<CoreQtAdapter>();

    // Registrar observador SOLO si hay un core válido
    if (coreSistema)
    {
        coreSistema->registrarObservador(coreAdapter);
    }

    // Conectar señales y slots
    conectarSenalesYSlots();

    // Conectar click en lista de productos a slot que agrega al pedido
    connect(ui->listaMenuProductos, &QListWidget::itemClicked, this, &MainWindow::agregarProductoAlPedido);

    // Inicializar estado del total y del botón
    m_totalActual = 0.0;
    ui->lblTotal->setText(QString("$ %1").arg(m_totalActual, 0, 'f', 2));
    ui->btnFinalizarPedido->setEnabled(false);

    // Conectar la señal del proxy (si existe) para procesar respuestas
    if (m_core)
    {
        QObject *obj = dynamic_cast<QObject *>(m_core);
        if (obj)
        {
            connect(obj, SIGNAL(respuestaRecibida(QJsonObject)), this, SLOT(procesarRespuesta(QJsonObject)));
        }
    }

    // Cargar datos iniciales en la UI (si hay core)
    if (coreSistema)
    {
        cargarMenuEnUI();
        cargarPedidosEnUI();

        // Popular descuentos
        ui->comboDescuentos->clear();
        auto descuentos = coreSistema->getDescuentosDisponibles();
        for (const auto &desc : descuentos)
        {
            ui->comboDescuentos->addItem(QString::fromStdString(desc.descripcion),
                                         QString::fromStdString(desc.id_descuento));
        }
    }

    // Si el core es concretamente un NetworkClientProxy, conectamos su señal de respuestas
    if (m_core)
    {
        // Intentar hacer cast dinámico
        QObject *obj = dynamic_cast<QObject *>(m_core);
        if (obj)
        {
            // Usamos Qt::DirectConnection ya que todo corre en el mismo hilo por ahora
            connect(obj, SIGNAL(respuestaRecibida(QJsonObject)), this, SLOT(procesarRespuesta(QJsonObject)), Qt::DirectConnection);
            connect(obj, SIGNAL(errorOcurrido(QString)), this, SLOT(onCoreError(QString)), Qt::QueuedConnection);
        }
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

    //
    connect(coreAdapter.get(), &CoreQtAdapter::pedidoTerminado,
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
    // Limpiar las 3 listas
    ui->listEnCola->clear();
    ui->listProcesando->clear();
    ui->listListo->clear();

    auto pedidos = coreSistema->getPedidosActivos();

    for (const auto &pedido : pedidos)
    {
        QString textoTarjeta = QString("Pedido #%1\nCliente: %2\n")
                                   .arg(pedido.id_pedido)
                                   .arg(QString::fromStdString(pedido.cliente));
        for (auto item : pedido.items)
        {
            textoTarjeta += QString("\n• %1 x%2")
                                .arg(QString::fromStdString(item.nombreProducto))
                                .arg(item.cantidad);
        }

        QListWidgetItem *tarjeta = new QListWidgetItem(textoTarjeta);

        // Clasificacion de estado
        if (pedido.estado == "En Cola")
        {
            ui->listEnCola->addItem(tarjeta);
        }
        else if (pedido.estado == "En Preparación")
        {
            tarjeta->setBackground(QColor("#fff3cd")); // Color de fondo amarillo claro
            ui->listProcesando->addItem(tarjeta);
        }
        else if (pedido.estado == "Listo")
        {

            tarjeta->setBackground(QColor("#d4edda")); // Color de fondo verde claro
            ui->listListo->addItem(tarjeta);
        }
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
    // Reuse agregarProductoAlPedido if an item is selected
    QListWidgetItem *itemSeleccionado = ui->listaMenuProductos->currentItem();
    if (!itemSeleccionado)
    {
        onCoreError("Por favor, seleccione un producto del menú.");
        return;
    }
    agregarProductoAlPedido(itemSeleccionado);
}

void MainWindow::agregarProductoAlPedido(QListWidgetItem *item)
{
    if (!item)
        return;

    int id = item->data(Qt::UserRole).toInt();
    double precio = item->data(Qt::UserRole + 1).toDouble();
    QString nombre = item->text();

    // Crear la representación en la lista del pedido
    QString texto = QString("1x %1").arg(nombre);
    QListWidgetItem *newItem = new QListWidgetItem(texto);
    newItem->setData(Qt::UserRole, id);
    newItem->setData(Qt::UserRole + 1, precio);

    ui->listaOrdenActual->addItem(newItem);

    // Actualizar total
    m_totalActual += precio;
    ui->lblTotal->setText(QString("$ %1").arg(m_totalActual, 0, 'f', 2));

    // Habilitar botón de finalizar si el total > 0
    ui->btnFinalizarPedido->setEnabled(m_totalActual > 0.0);
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

void MainWindow::procesarRespuesta(const QJsonObject &obj)
{
    qDebug() << "MainWindow::procesarRespuesta - recibida respuesta:" << QJsonDocument(obj).toJson(QJsonDocument::Compact);

    // Verificamos si el objeto contiene la clave 'productos' (respuesta de GET_MENU)
    QJsonArray productos;
    if (obj.contains("productos") && obj.value("productos").isArray())
    {
        productos = obj.value("productos").toArray();
    }
    else if (obj.contains(Protocolo::KEY_DATA) && obj.value(Protocolo::KEY_DATA).isArray())
    {
        // Compatibilidad: algunos responses usan 'data' con un array de productos
        productos = obj.value(Protocolo::KEY_DATA).toArray();
    }
    else
    {
        qDebug() << "MainWindow::procesarRespuesta - no es una respuesta de productos, ignorando";
        return; // No es respuesta GET_MENU
    }

    // Llenar la lista de productos en la UI
    ui->listaMenuProductos->clear();

    for (const QJsonValue &v : productos)
    {
        if (!v.isObject())
            continue;
        QJsonObject p = v.toObject();
        int id = p.contains("id") ? p.value("id").toInt() : -1;
        QString nombre = p.contains("nombre") ? p.value("nombre").toString() : QString("Producto");
        double precio = p.contains("precio") ? p.value("precio").toDouble() : 0.0;

        QString texto = QString("%1 - S/ %2").arg(nombre).arg(precio, 0, 'f', 2);
        QListWidgetItem *item = new QListWidgetItem(texto);
        item->setData(Qt::UserRole, id);
        // Guardar el precio también en UserRole+1 para uso posterior
        item->setData(Qt::UserRole + 1, precio);
        ui->listaMenuProductos->addItem(item);
    }
}

// IObservadorCore implementation
void MainWindow::onNuevosPedidosEnCola()
{
    qDebug() << "MainWindow::onNuevosPedidosEnCola - llamado por el core";
    // Actualizar UI de manera segura
    QMetaObject::invokeMethod(this, "onPedidosActualizados", Qt::QueuedConnection);
}

void MainWindow::onPedidoTerminado(int /*id_pedido*/)
{
    qDebug() << "MainWindow::onPedidoTerminado - actualizar lista";
    QMetaObject::invokeMethod(this, "onPedidosActualizados", Qt::QueuedConnection);
}

void MainWindow::onError(const std::string &mensaje)
{
    qDebug() << "MainWindow::onError -" << QString::fromStdString(mensaje);
    QMetaObject::invokeMethod(this, "onCoreError", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(mensaje)));
}
