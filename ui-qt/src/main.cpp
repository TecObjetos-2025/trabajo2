#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- ESTILO "LA ESQUINA MODERNA" ---
    // Colores extraídos de tu diseño HTML/Tailwind
    // Primary (Azul): #387ADF | Accent (Naranja): #F5821F | Background: #F8F9FA

    a.setStyleSheet(R"(
        /* Fondo General de la Aplicación */
        QMainWindow {
            background-color: #F8F9FA;
        }

        /* Paneles y Pestañas (Simulando las tarjetas blancas) */
        QTabWidget::pane {
            border: 1px solid #dee2e6;
            background-color: #FFFFFF;
            border-radius: 12px;
            margin-top: -1px; /* Para fusionar con la barra de pestañas */
        }

        /* Barra de Pestañas */
        QTabBar::tab {
            background: #F8F9FA;
            border: 1px solid #dee2e6;
            border-bottom-color: #dee2e6; /* Sin seleccionar */
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            padding: 10px 20px;
            margin-right: 4px;
            color: #6c757d;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            background: #FFFFFF;
            border-bottom-color: #FFFFFF; /* Se fusiona con el panel */
            color: #387ADF; /* Azul Primario */
        }

        /* Listas (Menú y Cola) */
        QListWidget {
            background-color: #FFFFFF;
            border: 1px solid #e9ecef;
            border-radius: 8px;
            outline: none;
            padding: 5px;
        }
        QListWidget::item {
            padding: 12px;
            border-bottom: 1px solid #f8f9fa;
            color: #111813;
        }
        QListWidget::item:selected {
            background-color: #387ADF;
            color: white;
            border-radius: 6px;
        }
        QListWidget::item:hover {
            background-color: #f1f3f5;
            color: #111813; /* Mantener texto oscuro en hover */
        }

        /* Botones Generales */
        QPushButton {
            background-color: #FFFFFF;
            border: 1px solid #dee2e6;
            border-radius: 8px;
            padding: 8px 16px;
            color: #495057;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #e9ecef;
        }

        QPushButton#btnQuitarItem {
            background-color: transparent;
            color: #dc3545; /* Rojo */
            border: 1px solid #dc3545;
        }
        QPushButton#btnQuitarItem:hover {
            background-color: #dc3545;
            color: white;
        }

        /* Botón de Acción Principal ("Finalizar Pedido" - Naranja) */
        /* Nota: Asegúrate de que el botón en el UI se llame 'btnFinalizarPedido' */
        QPushButton#btnFinalizarPedido {
            background-color: #F5821F;
            color: white;
            border: none;
            font-size: 14px;
            padding: 12px;
        }
        QPushButton#btnFinalizarPedido:hover {
            background-color: #e07215;
        }
        QPushButton#btnFinalizarPedido:pressed {
            background-color: #c7610e;
        }

        /* Botón Secundario ("Añadir Item" - Azul) */
        QPushButton#btnAnadirItem {
            background-color: #387ADF;
            color: white;
            border: none;
        }
        QPushButton#btnAnadirItem:hover {
            background-color: #2c65c2;
        }

        /* Inputs y Combos */
        QLineEdit, QComboBox {
            background-color: #FFFFFF;
            border: 1px solid #ced4da;
            border-radius: 6px;
            padding: 6px;
            color: #495057;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 2px solid #387ADF;
        }
    )");
    MainWindow w;
    w.show();
    return a.exec();
}
