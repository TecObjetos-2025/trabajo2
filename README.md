# Sistema de Gestión de Pedidos para Cafetería (C++)

**Integrantes**

- Fabricio Alonso Balarezo Delgado
- Juan Manuel Cari Quispe
- Anthony Ronaldo Cahui Benegas

## Descripción general

Aplicación de consola en C++ que implementa un sistema de gestión de pedidos para una cafetería, siguiendo principios de Programación Orientada a Objetos (POO) y buenas prácticas. Permite:

- **Gestionar clientes**: registro y búsqueda por ID.
- **Administrar menú**: alta de productos y visualización del menú.
- **Crear pedidos**: agregar ítems (producto + cantidad), calcular total y cambiar estado.
- **Notificar a la cocina**: mediante el patrón Observer al finalizar o actualizar pedidos.

Se incluye una versión modular (en `src/`) y una versión monolítica de ejemplo (`main.cpp` en la raíz) para comparación didáctica.

## Estructura de carpetas

```
trabajo2/
├─ src/
│  ├─ main.cpp                  # Punto de entrada principal (arquitectura modular)
│  ├─ Cliente.h / Cliente.cpp
│  ├─ Producto.h / Producto.cpp
│  ├─ ItemPedido.h / ItemPedido.cpp
│  ├─ Pedido.h / Pedido.cpp
│  ├─ MenuCafeteria.h / MenuCafeteria.cpp
│  ├─ Observador.h              # Interfaz del patrón Observer
│  ├─ Cocina.h / Cocina.cpp     # Observador concreto
│  ├─ SistemaPedidos.h / SistemaPedidos.cpp
│  └─ cafeteria/                # (Reservado para recursos adicionales)
├─ main.cpp                     # Ejemplo monolítico en un solo archivo
├─ README.md
├─ LICENSE
└─ .vscode/
```

## Tecnologías

- **Lenguaje**: C++ (C++17 recomendado)
- **Estandar**: STL (`<vector>`, `<string>`, `<iostream>`, etc.)
- **Compilador**: `g++` o compatible (probado en Linux)
- **No requiere dependencias externas**

## Arquitectura y diseño (POO)

- **Clases principales**:

  - `Producto`: entidad del catálogo (id, nombre, precio, categoría).
  - `ItemPedido`: ítem compuesto por `Producto` y cantidad; calcula subtotal.
  - `Cliente`: datos básicos del cliente (id, nombre, teléfono).
  - `Pedido`: compuesto por `ItemPedido`, asociado a un `Cliente`, gestiona estado y total.
  - `MenuCafeteria`: repositorio interno de `Producto`, búsqueda por id y visualización.
  - `SistemaPedidos`: capa orquestadora; registra clientes, crea pedidos, delega en `MenuCafeteria`, y notifica observadores.
  - `Observador` (interfaz) y `Cocina` (implementación): patrón Observer para reaccionar a cambios en `Pedido`.

- **Patrones y principios**:
  - **Observer**: `SistemaPedidos` notifica a los observadores (p.ej., `Cocina`) cuando se actualiza/finaliza un `Pedido`.
  - **Encapsulamiento**: atributos privados con getters/setters controlados.
  - **Única responsabilidad**: cada clase tiene una función clara.
  - **Delegación**: `SistemaPedidos` delega la gestión de productos a `MenuCafeteria`.

## Configuración y ejecución

Requisitos:

- Linux con `g++` (GCC 9+ recomendado)
- Make no es requerido (no hay Makefile)

### 1) Compilar y ejecutar la versión modular (recomendada)

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -I./src \
  ./src/*.cpp -o build/cafeteria
./build/cafeteria
```

Punto de entrada: `src/main.cpp`.

Flujo en la consola:

- 1: Mostrar menú de la cafetería
- 2: Registrar nuevo cliente (se genera un ID incremental)
- 3: Crear nuevo pedido (agregar productos por ID; finaliza con -1)
- 4: Salir

Durante la creación del pedido, al finalizar se **notifica a cocina** vía Observer.

### 2) Compilar y ejecutar la versión monolítica (opcional, demostrativa)

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o build/demo
./build/demo
```

Esta variante concentra todas las clases en un único archivo para fines didácticos.

## Pruebas

Actualmente no se incluyen pruebas automatizadas. Recomendaciones para validación manual:

- Registrar uno o más clientes y crear pedidos con múltiples productos.
- Probar entradas inválidas (IDs inexistentes, cantidades <= 0) para verificar mensajes.
- Verificar que el total del pedido corresponde a la suma de subtotales.
- Confirmar que la notificación a `Cocina` aparece al finalizar/actualizar un pedido.

Sugerencias futuras: integrar un framework de pruebas (p. ej., GoogleTest) y añadir pruebas unitarias para `Pedido::calcularTotal`, `MenuCafeteria::getProductoPorId`, etc.

## Licencia

Este proyecto se distribuye bajo la licencia incluida en `LICENSE`.
