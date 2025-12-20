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

    // Inicializar estado del total y del botón
    m_totalActual = 0.0;
    ui->lblTotal->setText(QString("$ %1").arg(m_totalActual, 0, 'f', 2));
    ui->lblSubtotal->setText(QString("$ %1").arg(0.0, 0, 'f', 2));
    ui->lblIGV->setText(QString("$ %1").arg(0.0, 0, 'f', 2));
    ui->btnFinalizarPedido->setEnabled(false);

    // NOTE: No conectar itemClicked -> usamos solo botones btnAnadirItem/btnQuitarItem

    // El botón 'Finalizar' se conecta automáticamente por Qt a on_btnFinalizarPedido_clicked()
    // Evitar conexión manual doble que causaba ejecuciones duplicadas

    // Cargar datos iniciales en la UI (si hay core)
    if (coreSistema)
    {
        // Cargar sin que el slot procesarRespuesta intercepte la respuesta síncrona (evita duplicados)
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
            // Nota: conectar DESPUÉS de cargar el menú evita que la respuesta síncrona cause duplicados
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
        // Mostrar consistentemente: "Nombre - S/ 5.00"
        QString itemText = QString("%1 - S/ %2")
                               .arg(QString::fromStdString(producto.nombre))
                               .arg(producto.precio, 0, 'f', 2);
        QListWidgetItem *item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, producto.id);         // Almacenar el ID del producto
        item->setData(Qt::UserRole + 1, producto.precio); // Guardar precio también
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

    double tasaIGV = 0.0;
    if (coreSistema)
        tasaIGV = coreSistema->getPorcentajeIGV();

    double montoIGV = subtotal * tasaIGV;
    double total = subtotal + montoIGV;

    // Actualizacion visual (usar mismo formato dólar para consistencia)
    ui->lblSubtotal->setText(QString("$ %1").arg(subtotal, 0, 'f', 2));
    ui->lblIGV->setText(QString("$ %1").arg(montoIGV, 0, 'f', 2));

    // Total en negrita
    ui->lblTotal->setText(QString("$ %1").arg(total, 0, 'f', 2));

    // Actualizar miembro
    m_totalActual = total;

    // Habilitar/deshabilitar el botón
    ui->btnFinalizarPedido->setEnabled(m_totalActual > 0.0);
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

    // Recalcular totales usando la función centralizada
    actualizarTotalesUI();

    // Habilitar botón de finalizar si subtotal > 0
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
    // Para compatibilidad llamamos a finalizarPedido que contiene la lógica nueva
    finalizarPedido();
}

void MainWindow::finalizarPedido()
{
    qDebug() << "MainWindow::finalizarPedido - preparando pedido para envío";

    QString clienteQ = ui->lineEditCliente->text();
    if (clienteQ.trimmed().isEmpty())
    {
        onCoreError("Por favor, ingrese el nombre del cliente.");
        return;
    }

    // Agrupar por producto (id -> cantidad)
    QMap<int, int> counts;
    for (int i = 0; i < ui->listaOrdenActual->count(); ++i)
    {
        QListWidgetItem *itm = ui->listaOrdenActual->item(i);
        int productoId = itm->data(Qt::UserRole).toInt();
        counts[productoId] = counts.value(productoId, 0) + 1;
    }

    if (counts.isEmpty())
    {
        onCoreError("El pedido no puede estar vacío.");
        return;
    }

    // Construir vector<ItemPedidoCrear>
    std::vector<ItemPedidoCrear> itemsVec;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it)
    {
        ItemPedidoCrear itc;
        itc.productoId = it.key();
        itc.cantidad = it.value();
        itemsVec.push_back(itc);
    }

    // Obtener id_descuento
    std::string id_descuento = ui->comboDescuentos->currentData().toString().toStdString();

    // Mostrar JSON para depuración
    QJsonObject payload;
    payload.insert("cliente", clienteQ);
    QJsonArray arr;
    for (const auto &it : itemsVec)
    {
        QJsonObject o;
        o.insert("productoId", it.productoId);
        o.insert("cantidad", it.cantidad);
        arr.append(o);
    }
    payload.insert("items", arr);
    payload.insert("id_descuento", QString::fromStdString(id_descuento));
    qDebug() << "MainWindow::finalizarPedido - JSON payload:" << QJsonDocument(payload).toJson(QJsonDocument::Compact);

    // Enviar usando la API del core (que en el caso del proxy hará la llamada de red)
    try
    {
        if (coreSistema)
        {
            coreSistema->finalizarPedido(clienteQ.toStdString(), itemsVec, id_descuento);
        }
        else if (m_core)
        {
            // fallback: intentar llamar al raw pointer
            m_core->finalizarPedido(clienteQ.toStdString(), itemsVec, id_descuento);
        }
        else
        {
            onCoreError("No hay core conectado para enviar el pedido.");
            return;
        }
    }
    catch (const std::exception &ex)
    {
        onCoreError(ex.what());
        return;
    }

    // Limpiar UI
    ui->listaOrdenActual->clear();
    ui->lineEditCliente->clear();
    ui->comboDescuentos->setCurrentIndex(0);
    m_totalActual = 0.0;
    ui->lblTotal->setText(QString("$ %1").arg(m_totalActual, 0, 'f', 2));
    ui->btnFinalizarPedido->setEnabled(false);

    QMessageBox::information(this, "Pedido Enviado", "Pedido enviado correctamente al servidor.");
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
        // Compatibilidad: algunos responses usan 'data' con un array. Pero ojo: ese 'data' puede
        // contener pedidos (GET_ORDERS) o productos (GET_MENU). Debemos detectar si realmente
        // se trata de productos (id/nombre/precio) para no reemplazar el menú con pedidos.
        QJsonArray arr = obj.value(Protocolo::KEY_DATA).toArray();
        if (!arr.isEmpty() && arr.at(0).isObject())
        {
            QJsonObject first = arr.at(0).toObject();
            if (first.contains("id") && first.contains("nombre") && first.contains("precio"))
            {
                productos = arr;
            }
            else
            {
                qDebug() << "MainWindow::procesarRespuesta - KEY_DATA recibido pero no parecen ser productos, ignorando para menu";
                return; // No es respuesta GET_MENU
            }
        }
        else
        {
            qDebug() << "MainWindow::procesarRespuesta - KEY_DATA vacío o no es object, ignorando";
            return;
        }
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
