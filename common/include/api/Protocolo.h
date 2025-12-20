#pragma once
#include <QString>

namespace Protocolo
{
    // Puertos
    static const int DEFAULT_PORT = 1234;

    // Claves del JSON
    static const QString KEY_CMD = "cmd";         // Qué quieres hacer
    static const QString KEY_PAYLOAD = "payload"; // Datos (Argumentos)
    static const QString KEY_STATUS = "status";   // OK / ERROR
    static const QString KEY_MSG = "message";     // Mensaje de error o éxito
    static const QString KEY_DATA = "data";       // Respuesta (ej: el menú)

    // Comandos (Requests)
    static const QString CMD_LOGIN = "LOGIN";
    static const QString CMD_GET_MENU = "GET_MENU";
    static const QString CMD_ADD_ORDER = "ADD_ORDER";
    // Nuevo comando para obtener pedidos activos (vista Cocina)
    static const QString CMD_GET_ORDERS = "GET_ORDERS";

    // Eventos (Push Notifications)
    static const QString EVT_NEW_ORDER = "EVT_NEW_ORDER";
    static const QString EVT_KITCHEN_UPDATE = "EVT_KITCHEN_UPDATE";
}