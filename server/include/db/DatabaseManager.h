#pragma once
#include <QtSql/QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager &instance();
    bool inicializarTablas();
    bool seedData();
    QSqlDatabase &database();

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    QSqlDatabase m_db;
};
