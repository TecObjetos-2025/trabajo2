#include <gtest/gtest.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "db/DatabaseManager.h"

// Test Singleton
TEST(DatabaseManagerTest, SingletonInstance)
{
    DatabaseManager &db1 = DatabaseManager::instance();
    DatabaseManager &db2 = DatabaseManager::instance();
    EXPECT_EQ(&db1, &db2);
}

// Test inicializarTablas crea tablas sin error
TEST(DatabaseManagerTest, InicializarTablas)
{
    DatabaseManager &db = DatabaseManager::instance();
    EXPECT_TRUE(db.inicializarTablas());
}

// Test seedData inserta datos de prueba
TEST(DatabaseManagerTest, SeedData)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.inicializarTablas();
    EXPECT_TRUE(db.seedData());
}
