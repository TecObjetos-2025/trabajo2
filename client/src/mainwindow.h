#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <memory>

// Inclusión de los DTOs y la interfaz del core
#include "api/ICoreSistema.h"
#include "api/ApiDTOs.h"

// Clases de implementación del core
#include "CoreQtAdapter.h" // 'Traductor' de observadores C++ a señales Qt

// Interfaces MVP
#include "presenter/interfaces/ICajeroView.h"
#include "presenter/interfaces/ICocinaView.h"
#include "presenter/CajeroPresenter.h"
#include "presenter/CocinaPresenter.h"

#include <QListWidgetItem>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IObservadorCore, public ICajeroView, public ICocinaView
{
    Q_OBJECT

    enum ItemDataRoles
    {
        RoleProductoId = Qt::UserRole,
        RolePrecioUnitario = Qt::UserRole + 1,
        RoleProductoNombre = Qt::UserRole + 2
    };

public:
    // Nueva firma del constructor que recibe un puntero crudo al core
    MainWindow(ICoreSistema *core, QWidget *parent = nullptr);
    ~MainWindow();

    // --- ICajeroView Implementation ---
    void showMenu(const std::vector<ProductViewModel>& products) override;
    void showCurrentOrder(const std::vector<OrderItemViewModel>& items) override;
    void updateTotals(const std::string& subtotal, const std::string& igv, const std::string& total) override;
    void showMessage(const std::string& title, const std::string& msg) override;
    void showDiscounts(const std::vector<InfoDescuento>& discounts) override;
    void enableFinalizeButton(bool enable) override;
    std::string getClientName() override;
    std::string getSelectedDiscountId() override;
    void clearForm() override;

    // --- ICocinaView Implementation ---
    void clearKanbanBoard() override;
    void addOrderToColumn(const std::string& status, const std::string& cardContent) override;
    void showKitchenMessage(const std::string& msg) override;

private:
    Ui::MainWindow *ui;

    // Componentes 'backend'

    // Interfaz de desacoplamiento con el core del sistema (usado por el UI)
    std::shared_ptr<ICoreSistema> coreSistema;

    // Puntero crudo al core inyectado (no responsable de la vida)
    ICoreSistema *m_core = nullptr;

    // "Traductor" de observadores C++ a señales Qt
    std::shared_ptr<CoreQtAdapter> coreAdapter;

    // MVP Presenters
    std::unique_ptr<CajeroPresenter> mCajeroPresenter;
    std::unique_ptr<CocinaPresenter> mCocinaPresenter;

    // Métodos auxiliares
    /**
     * @brief conectarSenalesYSlots Conecta las señales y slots necesarios entre la UI y el core.
     */
    void conectarSenalesYSlots();

private slots:
    // Slots "Vista Cajero"
    /**
     * @brief on_btnFinalizarPedido_clicked Slot llamado al hacer clic en el botón "Finalizar Pedido".
     */
    void on_btnFinalizarPedido_clicked();

    /**
     * @brief on_btnAnadirItem_clicked Slot llamado al hacer clic en el botón "Añadir Ítem".
     */
    void on_btnAnadirItem_clicked();

    /**
     * @brief on_btnQuitarItem_clicked Slot llamado al hacer clic en el botón "Quitar Ítem".
     */
    void on_btnQuitarItem_clicked();

    // Slots "Vista Cocina"
    /**
     * @brief Se activa cuando el cocinero presiona "Procesar Siguiente Pedido".
     */
    void on_btnProcesarSiguientePedido_clicked();

    // Slots para manejar las señales del CoreQtAdapter
    /**
     * @brief se activa cuando el core notifica que la cola de pedidos ha cambiado.
     * Traduce en una actualización de la UI.
     */
    void onPedidosActualizados();

    /**
     * @brief Se activa cuando el core notifica que ocurrió un error.
     */
    void onCoreError(QString mensaje);

    // Procesar respuestas que provengan del proxy (respuestaRecibida)
    void procesarRespuesta(const QJsonObject &obj);

    // IObservadorCore implementation (para poder registrarnos como observador)
    void onNuevosPedidosEnCola() override;
    void onPedidoTerminado(int id_pedido) override;
    void onError(const std::string &mensaje) override;
};
#endif // MainWindow_H
