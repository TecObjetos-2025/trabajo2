#include <string>
#include <vector>

/**
 * @file ApiDTOs.h
 * @brief Definición de estructuras de datos para la API del sistema de pedidos.
 * Estas estructuras son utilizadas para transferir información entre
 * diferentes componentes del sistema, como la interfaz de usuario y el núcleo del sistema.
 */

//-------------------------------------------------
// ESTRUCTURAS DE DATOS PARA LA API
//-------------------------------------------------

struct InfoProducto
{
    int id;
    std::string nombre;
    double precio;
};

struct InfoDescuento
{
    std::string id_descuento; // Ej: "2X1_BEBIDAS"
    std::string descripcion;  // Ej: "2x1 en Bebidas"
};

// Datos que la UI necesita para *crear* un item
struct ItemPedidoCrear
{
    int productoId;
    int cantidad;
};

// Datos que la UI necesita para *mostrar* un item
struct ItemPedidoInfo
{
    std::string nombreProducto;
    int cantidad;
    double precioUnitario;
};

struct InfoPedido
{
    int id_pedido;
    std::string cliente;
    std::string estado; // Ej: "En Cola", "En Preparación", "Listo"
    std::vector<ItemPedidoInfo> items;
    double total_final;
};