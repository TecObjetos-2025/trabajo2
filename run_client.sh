#!/usr/bin/env bash
# Wrapper para ejecutar CafeteriaClient evitando conflicto con librerías de snap (libpthread)
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
CLIENT_BIN="$ROOT_DIR/build/client/CafeteriaClient"
if [ ! -x "$CLIENT_BIN" ]; then
  echo "ERROR: no se encontró el binario: $CLIENT_BIN" >&2
  echo "Compila con: cmake --build build --target CafeteriaClient" >&2
  exit 1
fi
# Forzar uso de la pthread del sistema antes que la de snap
export LD_PRELOAD="/lib/x86_64-linux-gnu/libpthread.so.0"
# Optionally unset LD_LIBRARY_PATH if set by the environment (QtCreator snap)
unset LD_LIBRARY_PATH || true
exec "$CLIENT_BIN" "$@"
