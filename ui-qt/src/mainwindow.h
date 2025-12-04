#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

// Inclusión de los DTOs y la interfaz del core
#include "api/ICoreSistema.h"
#include "api/ApiDTOs.h"

// Clases de implementación del core
#include "CoreQtAdapter.h" // 'Traductor' de observadores C++ a señales Qt

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Componentes 'backend'

    // Interfaz de desacoplamiento con el core del sistema
    std::shared_ptr<ICoreSistema> coreSistema;

    // "Traductor" de observadores C++ a señales Qt
    std::shared_ptr<CoreQtAdapter> coreAdapter;

    // Métodos auxiliares

    /**
     * @brief cargarMenuEnUI Carga el menú de productos desde el core hacia la UI.
     * Vista Cajero.
     */
    void cargarMenuEnUI();

    /**
     * @brief cargarPedidosEnUI Carga la lista de pedidos en cola desde el core hacia la UI.
     * Vista Cocina.
     */
    void cargarPedidosEnUI();

    /**
     * @brief actualizarTotalesUI Actualiza los totales del pedido actual en la UI.
     */
    void actualizarTotalesUI();

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
};
#endif // MainWindow_H
