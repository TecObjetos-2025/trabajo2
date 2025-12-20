#include "repositories/ClienteRepository.h"
#include "models/Cliente.h"
#include "db/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QDebug>

using namespace repos;

ClienteRepository::ClienteRepository() {}
ClienteRepository::~ClienteRepository() {}

std::shared_ptr<Cliente> ClienteRepository::findByName(const std::string &name)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT id, name, tax_id FROM customers WHERE name = ?");
    q.addBindValue(QString::fromStdString(name));
    if (!q.exec())
    {
        qWarning() << "ClienteRepository::findByName - query failed:" << q.lastError().text();
        return nullptr;
    }
    if (q.next())
    {
        int id = q.value(0).toInt();
        QString qname = q.value(1).toString();
        QString tax = q.value(2).toString();
        return std::make_shared<Cliente>(id, qname.toStdString(), tax.toStdString());
    }
    return nullptr;
}

std::shared_ptr<Cliente> ClienteRepository::create(const std::string &name, const std::string &taxId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO customers (name, tax_id) VALUES (?, ?)");
    q.addBindValue(QString::fromStdString(name));
    q.addBindValue(QString::fromStdString(taxId));
    if (!q.exec())
    {
        qWarning() << "ClienteRepository::create - insert failed:" << q.lastError().text();
        return nullptr;
    }
    int id = q.lastInsertId().toInt();
    return std::make_shared<Cliente>(id, name, taxId);
}
