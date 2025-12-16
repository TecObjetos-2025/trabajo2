#include "db/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(":memory:"); // Cambiar a archivo si se requiere persistencia real
    if (!m_db.open())
    {
        qWarning() << "No se pudo abrir la base de datos:" << m_db.lastError().text();
    }
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

QSqlDatabase &DatabaseManager::database()
{
    return m_db;
}

bool DatabaseManager::inicializarTablas()
{
    // Esqueleto: solo retorna true por ahora
    return true;
}

bool DatabaseManager::seedData()
{
    // Esqueleto: solo retorna true por ahora
    return true;
}
