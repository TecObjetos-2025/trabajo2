#include <iostream>
#include <vector>
#include <string>
using namespace std;

// -------------------- Producto --------------------
class Producto {
private:
    int id;
    string nombre;
    double precio;
    string categoria;

public:
    Producto(int id, string nombre, double precio, string categoria)
        : id(id), nombre(nombre), precio(precio), categoria(categoria) {}

    // Getters
    int getId() const { return id; }
    string getNombre() const { return nombre; }
    double getPrecio() const { return precio; }
    string getCategoria() const { return categoria; }

    // Setter (solo para precio como ejemplo de modificación controlada)
    void setPrecio(double nuevoPrecio) { precio = nuevoPrecio; }
};

// -------------------- ItemPedido --------------------
class ItemPedido {
private:
    Producto producto;
    int cantidad;

public:
    ItemPedido(Producto producto, int cantidad)
        : producto(producto), cantidad(cantidad) {}

    double obtenerSubtotal() const {
        return producto.getPrecio() * cantidad;
    }

    Producto getProducto() const { return producto; }
    int getCantidad() const { return cantidad; }
};

// -------------------- Pedido --------------------
class Pedido {
private:
    int id;
    double total;
    string estado;
    vector<ItemPedido> items;

public:
    Pedido(int id) : id(id), total(0), estado("EN_PROCESO") {}

    void agregarItem(Producto producto, int cantidad) {
        items.push_back(ItemPedido(producto, cantidad));
    }

    void calcularTotal() {
        total = 0;
        for (auto &item : items) {
            total += item.obtenerSubtotal();
        }
    }

    void marcarComoPagado() { estado = "PAGADO"; }

    // Getters
    int getId() const { return id; }
    double getTotal() const { return total; }
    string getEstado() const { return estado; }
    vector<ItemPedido> getItems() const { return items; }
};

// -------------------- Cliente --------------------
class Cliente {
private:
    int id;
    string nombre;
    string telefono;

public:
    Cliente(int id, string nombre, string telefono)
        : id(id), nombre(nombre), telefono(telefono) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    string getTelefono() const { return telefono; }
};

// -------------------- MenuCafeteria --------------------
class MenuCafeteria {
private:
    vector<Producto> productosDisponibles;

public:
    void agregarProducto(Producto producto) {
        productosDisponibles.push_back(producto);
    }

    Producto obtenerProducto(int id) {
        for (auto &p : productosDisponibles) {
            if (p.getId() == id) return p;
        }
        throw runtime_error("Producto no encontrado");
    }

    void mostrarMenu() {
        cout << "---- Menu de Cafeteria ----" << endl;
        for (auto &p : productosDisponibles) {
            cout << p.getId() << ". " << p.getNombre()
                 << " - S/ " << p.getPrecio()
                 << " (" << p.getCategoria() << ")" << endl;
        }
    }
};

// -------------------- Observador --------------------
class Observador {
public:
    virtual void actualizar(Pedido pedido) = 0;
};

// -------------------- Cocina --------------------
class Cocina : public Observador {
public:
    void actualizar(Pedido pedido) override {
        cout << "[Cocina] Pedido #" << pedido.getId()
             << " actualizado con " << pedido.getItems().size()
             << " productos." << endl;
    }
};

// -------------------- SistemaPedidos --------------------
class SistemaPedidos {
private:
    vector<Pedido> pedidosActivos;
    vector<Cliente> clientesRegistrados;
    vector<Observador*> observadores;

public:
    Pedido& crearPedido(int idPedido) {
        pedidosActivos.push_back(Pedido(idPedido));
        return pedidosActivos.back();
    }

    void agregarProductoAPedido(Pedido &pedido, Producto producto, int cantidad) {
        pedido.agregarItem(producto, cantidad);
        pedido.calcularTotal();
        notificarCocina(pedido);
    }

    void finalizarPedido(Pedido &pedido) {
        pedido.marcarComoPagado();
        cout << "Pedido #" << pedido.getId()
             << " finalizado y marcado como PAGADO." << endl;
    }

    void agregarObservador(Observador* obs) {
        observadores.push_back(obs);
    }

    void notificarCocina(Pedido pedido) {
        for (auto &obs : observadores) {
            obs->actualizar(pedido);
        }
    }
};

// -------------------- MAIN con menú --------------------
int main() {
    SistemaPedidos sistema;
    Cocina cocina;
    sistema.agregarObservador(&cocina);

    MenuCafeteria menu;
    menu.agregarProducto(Producto(1, "Café Americano", 5.0, "Bebida"));
    menu.agregarProducto(Producto(2, "Sandwich", 8.5, "Comida"));
    menu.agregarProducto(Producto(3, "Jugo Natural", 6.0, "Bebida"));

    int opcion, cantidad, idPedido = 1;
    bool salir = false;

    Pedido &pedido = sistema.crearPedido(idPedido);

    while (!salir) {
        cout << "\n--- Sistema de Pedidos ---\n";
        cout << "1. Ver menu\n";
        cout << "2. Agregar producto al pedido\n";
        cout << "3. Finalizar pedido\n";
        cout << "4. Salir\n";
        cout << "5. Agregar nuevo producto al menú\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1:
            menu.mostrarMenu();
            break;
        case 2: {
            int idProducto;
            cout << "Ingrese ID del producto: ";
            cin >> idProducto;
            cout << "Cantidad: ";
            cin >> cantidad;

            try {
                Producto p = menu.obtenerProducto(idProducto);
                sistema.agregarProductoAPedido(pedido, p, cantidad);
                cout << "Producto agregado al pedido!" << endl;
            } catch (exception &e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 3:
            sistema.finalizarPedido(pedido);
            cout << "Total a pagar: S/ " << pedido.getTotal() << endl;
            salir = true;
            break;
        case 4:
            salir = true;
            break;
        case 5: {
            int idNuevo;
            string nombreNuevo, categoriaNueva;
            double precioNuevo;

            cout << "Ingrese ID del nuevo producto: ";
            cin >> idNuevo;
            cin.ignore();
            cout << "Nombre del producto: ";
            getline(cin, nombreNuevo);
            cout << "Precio: ";
            cin >> precioNuevo;
            cin.ignore();
            cout << "Categoria: ";
            getline(cin, categoriaNueva);

            menu.agregarProducto(Producto(idNuevo, nombreNuevo, precioNuevo, categoriaNueva));
            cout << "Producto agregado al menú exitosamente!\n";
            break;
        }
        default:
            cout << "Opcion invalida.\n";
        }
    }

    return 0;
}
