#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
            /* no-op deleter */ });
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

    // Initialize Presenters
    mCajeroPresenter = std::make_unique<CajeroPresenter>(this, m_core);
    mCocinaPresenter = std::make_unique<CocinaPresenter>(this, m_core);

    // Conectar señales y slots
    conectarSenalesYSlots();

    // Initial UI State
    ui->lblTotal->setText("$ 0.00");
    ui->lblSubtotal->setText("$ 0.00");
    ui->lblIGV->setText("$ 0.00");
    ui->btnFinalizarPedido->setEnabled(false);

    // Load Initial Data
    if (m_core) {
        mCajeroPresenter->loadInitialData();
        mCocinaPresenter->refreshKanban();
    }

    // Si el core es concretamente un NetworkClientProxy, conectamos su señal de respuestas
    if (m_core)
    {
        QObject *obj = dynamic_cast<QObject *>(m_core);
        if (obj)
        {
            // Note: We keep this for now to handle async responses that might need to trigger generic updates,
            // but specific logic is now in Presenters.
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
    connect(coreAdapter.get(), &CoreQtAdapter::nuevosPedidosEnCola,
            this, &MainWindow::onPedidosActualizados);

    connect(coreAdapter.get(), &CoreQtAdapter::pedidoTerminado,
            this, &MainWindow::onPedidosActualizados);

    connect(coreAdapter.get(), &CoreQtAdapter::errorOcurrido,
            this, &MainWindow::onCoreError);
}

// ----------------------------------------------------------------------------
// ICajeroView Implementation
// ----------------------------------------------------------------------------

void MainWindow::showMenu(const std::vector<ProductViewModel>& products) {
    ui->listaMenuProductos->clear();
    for (const auto& prod : products) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(prod.displayName));
        item->setData(RoleProductoId, prod.id);
        item->setData(RolePrecioUnitario, prod.price);
        item->setData(RoleProductoNombre, QString::fromStdString(prod.rawName));

        ui->listaMenuProductos->addItem(item);
    }
}

void MainWindow::showCurrentOrder(const std::vector<OrderItemViewModel>& items) {
    ui->listaOrdenActual->clear();
    for (const auto& item : items) {
        QListWidgetItem *listItem = new QListWidgetItem(QString::fromStdString(item.text));
        // We might not need to store data here anymore since Presenter holds state,
        // but it's useful if we wanted to implement item selection for removal by ID later.
        ui->listaOrdenActual->addItem(listItem);
    }
}

void MainWindow::updateTotals(const std::string& subtotal, const std::string& igv, const std::string& total) {
    ui->lblSubtotal->setText(QString::fromStdString(subtotal));
    ui->lblIGV->setText(QString::fromStdString(igv));
    ui->lblTotal->setText(QString::fromStdString(total));
}

void MainWindow::showMessage(const std::string& title, const std::string& msg) {
    QMessageBox::information(this, QString::fromStdString(title), QString::fromStdString(msg));
}

void MainWindow::showDiscounts(const std::vector<InfoDescuento>& discounts) {
    ui->comboDescuentos->clear();
    for (const auto& desc : discounts) {
        ui->comboDescuentos->addItem(QString::fromStdString(desc.descripcion),
                                     QString::fromStdString(desc.id_descuento));
    }
}

void MainWindow::enableFinalizeButton(bool enable) {
    ui->btnFinalizarPedido->setEnabled(enable);
}

std::string MainWindow::getClientName() {
    return ui->lineEditCliente->text().toStdString();
}

std::string MainWindow::getSelectedDiscountId() {
    return ui->comboDescuentos->currentData().toString().toStdString();
}

void MainWindow::clearForm() {
    ui->lineEditCliente->clear();
    ui->comboDescuentos->setCurrentIndex(0);
}

// ----------------------------------------------------------------------------
// ICocinaView Implementation
// ----------------------------------------------------------------------------

void MainWindow::clearKanbanBoard() {
    ui->listEnCola->clear();
    ui->listProcesando->clear();
    ui->listListo->clear();
}

void MainWindow::addOrderToColumn(const std::string& status, const std::string& cardContent) {
    QListWidgetItem *tarjeta = new QListWidgetItem(QString::fromStdString(cardContent));

    if (status == "En Cola") {
        ui->listEnCola->addItem(tarjeta);
    } else if (status == "En Preparación") {
        tarjeta->setBackground(QColor("#fff3cd"));
        ui->listProcesando->addItem(tarjeta);
    } else if (status == "Listo") {
        tarjeta->setBackground(QColor("#d4edda"));
        ui->listListo->addItem(tarjeta);
    }
}

void MainWindow::showKitchenMessage(const std::string& msg) {
    // Maybe show in status bar or popup
    // For now, popup
    QMessageBox::information(this, "Cocina", QString::fromStdString(msg));
}


// ----------------------------------------------------------------------------
// UI Slots (Redirect to Presenters)
// ----------------------------------------------------------------------------

void MainWindow::on_btnAnadirItem_clicked()
{
    QListWidgetItem *itemSeleccionado = ui->listaMenuProductos->currentItem();
    if (!itemSeleccionado) {
        onCoreError("Por favor, seleccione un producto del menú.");
        return;
    }

    int id = itemSeleccionado->data(RoleProductoId).toInt();
    double precio = itemSeleccionado->data(RolePrecioUnitario).toDouble();
    QString name = itemSeleccionado->data(RoleProductoNombre).toString();

    mCajeroPresenter->addToOrder(id, precio, name.toStdString());
}

void MainWindow::on_btnQuitarItem_clicked()
{
    int fila = ui->listaOrdenActual->currentRow();
    mCajeroPresenter->removeFromOrder(fila);
}

void MainWindow::on_btnFinalizarPedido_clicked()
{
    mCajeroPresenter->finalizeOrder();
}

void MainWindow::on_btnProcesarSiguientePedido_clicked()
{
    mCocinaPresenter->advanceOrder();
}


// --- Slots for Core Signals ---

void MainWindow::onPedidosActualizados()
{
    qDebug() << "[MainWindow] Slot 'onPedidosActualizados' activado.";
    mCocinaPresenter->refreshKanban();
}

void MainWindow::onCoreError(QString mensaje)
{
    qDebug() << "[MainWindow] Slot 'onCoreError' activado con mensaje:" << mensaje;
    QMessageBox::critical(this, "Error del Sistema", mensaje);
}

void MainWindow::procesarRespuesta(const QJsonObject &obj)
{
    // This used to handle Menu Loading via JSON.
    // Now Menu Loading is initiated by Presenter calling `getMenu()`.
    // However, if `getMenu()` is synchronous (which it seems to be in the Proxy interface,
    // although Proxy implementation might be waiting for socket), we are good.
    // If Proxy `getMenu` returns empty vector and relies on signal later, we have a problem.
    // Checking NetworkClientProxy: it sends request and waits for response (blocking).
    // So Presenter::loadInitialData() works fine.

    // We can keep this for logging or future push events not covered by IObservadorCore.
    qDebug() << "MainWindow::procesarRespuesta - recibida respuesta:" << QJsonDocument(obj).toJson(QJsonDocument::Compact);

    // If we receive a "menu updated" push event, we could call mCajeroPresenter->loadInitialData();
}

// IObservadorCore implementation
void MainWindow::onNuevosPedidosEnCola()
{
    QMetaObject::invokeMethod(this, "onPedidosActualizados", Qt::QueuedConnection);
}

void MainWindow::onPedidoTerminado(int /*id_pedido*/)
{
    QMetaObject::invokeMethod(this, "onPedidosActualizados", Qt::QueuedConnection);
}

void MainWindow::onError(const std::string &mensaje)
{
    QMetaObject::invokeMethod(this, "onCoreError", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(mensaje)));
}
