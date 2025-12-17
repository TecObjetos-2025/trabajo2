#pragma once
#include <QtSql/QSqlDatabase>
#include "api/ApiDTOs.h"

class DatabaseManager
{
public:
    static DatabaseManager &instance();
    bool inicializarTablas();
    bool seedData();
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
