#pragma once
#include <QtSql/QSqlDatabase>
#include "api/ApiDTOs.h"

class DatabaseManager
{
public:
    static DatabaseManager &instance();
    bool inicializarTablas();
    bool seedData();
    bool savePedido(const InfoPedido &pedidoDTO);
    QSqlDatabase &database();

    // Devuelve todos los productos activos como DTO
    std::vector<InfoProducto> getProductos();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    QSqlDatabase m_db;
};
