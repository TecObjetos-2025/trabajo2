# WBS‑3: Implementación - Eventos en Tiempo Real

## Resumen ejecutivo

Se implementó soporte de notificaciones en tiempo real en el sistema cliente‑servidor. Cuando un cliente finaliza un pedido, el servidor difunde `EVT_NEW_ORDER` a todos los clientes conectados; las UIs (vista Cocina) reciben el evento y solicitan `GET_ORDERS` para actualizar la lista de pedidos activos. Adicionalmente se añadieron mejoras en el cliente para distinguir eventos push de respuestas a peticiones y se añadieron tests de integración para validar el comportamiento multi‑cliente.

---

## Cambios principales y archivos clave

### Server

- `server/src/commands/CmdGetOrders.cpp` (nuevo)
- `server/include/commands/CmdGetOrders.h` (nuevo)
- `server/src/repositories/ClienteRepository.cpp` (nuevo)
- `server/include/repositories/ClienteRepository.h` (nuevo)
- `server/src/core/SistemaPedidos.cpp` (modificado: integración ClienteRepository, correcciones de observadores)
- `server/src/NetworkServer.cpp` (modificado: registro de comandos, método difundirEvento, logging)

### Client

- `client/src/NetworkClientProxy.cpp` (modificado: manejo de eventos, matching de respuestas, reintento en getPedidosActivos, helper de test)
- `client/src/NetworkClientProxy.h` (actualizado)
- `client/src/mainwindow.cpp` (modificado: validación de payload antes de repoblar menú)
- `client/CMakeLists.txt` (añadido target `run-client`)
- `run_client.sh` (nuevo: workaround LD_PRELOAD para Snap/libpthread)

### Tests

- `tests/test_network_proxy_async.cpp` (QtTest)
- `tests/test_network_proxy_orders.cpp` (QtTest)
- `tests/test_network_proxy_broadcast_integration.cpp` (QtTest)
- `tests/test_network_proxy_endtoend.cpp` (QtTest)
- `tests/test_network_broadcast.cpp` (gtest)
- `tests/CMakeLists.txt` (modificado: separar Qt tests y google test targets, añadir fuentes server cuando aplica)

### Docs

- `docs/WBS-3-Implementacion.md` (este archivo)
- `docs/DiagramaRed.md` (actualizado con sección "Eventos Push")

---

## Características añadidas

- Difusión de eventos `EVT_NEW_ORDER` por parte del servidor.
- Comando `GET_ORDERS` para obtener pedidos activos.
- Cliente detecta eventos push y notifica la UI; la UI luego solicita `GET_ORDERS` para refrescar.
- Matching de respuestas en el proxy (evita consumir respuestas no relacionadas con la petición actual).
- Tests de integración para validar broadcast multi‑cliente.

---

## Bugs y problemas resueltos

- Race en el primer `finalizarPedido` que provocaba timeout: solucionado con matching de respuestas, debounce en eventos y retry en `getPedidosActivos()`.
- Reemplazo del menú por payloads de pedidos (precio S/0.00): solucionado validando el formato de `KEY_DATA` antes de repoblar menú.
- Tests flakey por duplicidad de mains y fuentes faltantes: reorganizados targets de tests en CMake y añadidos archivos faltantes a test targets.

---

## Comandos para compilar y validar

- Compilar todo:

```bash
cmake -S . -B build
cmake --build build -- -j$(nproc)
```

- Ejecutar todos los tests:

```bash
ctest --test-dir build --output-on-failure
```

- Ejecutar tests individuales relevantes:

```bash
./build/tests/network_proxy_broadcast_integration -vs
./build/tests/network_proxy_endtoend -vs
./build/tests/network_proxy_orders -vs
```

- Ejecutar servidor y clientes manualmente:

```bash
./build/server/CafeteriaServer
./run_client.sh ./build/client/CafeteriaClient
# abrir dos instancias del cliente, finalizar pedido en cliente A y observar que cliente B reciba EVT_NEW_ORDER y haga GET_ORDERS
```

---

## Notas finales y recomendaciones

- Se recomienda planear un WBS‑4 para introducir `request_id` en requests/responses para eliminar las heurísticas de matching por contenido.
- Convertir operaciones síncronas del proxy a asíncronas para evitar bloquear la UI.
- Mejorar telemetría y logging (trace ids) para depuración en producción.

---

_Archivo generado automáticamente como parte del cierre de WBS-3._
