#include <gtest/gtest.h>

#include "db/DatabaseManager.h"
#include "repositories/ClienteRepository.h"
#include "models/Cliente.h"

using namespace repos;

TEST(ClienteRepositoryTest, FindExistingReturnsCliente)
{
    DatabaseManager::instance().inicializarTablas();
    ASSERT_TRUE(DatabaseManager::instance().seedData());

    ClienteRepository repo;
    auto c = repo.findByName("Carlos Juarez");
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->getNombre(), "Carlos Juarez");
    EXPECT_EQ(c->getTelefono(), "12345678");
}

TEST(ClienteRepositoryTest, FindNonExistingReturnsNull)
{
    DatabaseManager::instance().inicializarTablas();
    ASSERT_TRUE(DatabaseManager::instance().seedData());

    ClienteRepository repo;
    auto c = repo.findByName("NoExistente");
    EXPECT_EQ(c, nullptr);
}

TEST(ClienteRepositoryTest, CreateInsertsAndCanBeFound)
{
    DatabaseManager::instance().inicializarTablas();
    ASSERT_TRUE(DatabaseManager::instance().seedData());

    ClienteRepository repo;
    auto created = repo.create("Test Cliente", "99999999");
    ASSERT_NE(created, nullptr);
    EXPECT_EQ(created->getNombre(), "Test Cliente");
    EXPECT_EQ(created->getTelefono(), "99999999");

    auto found = repo.findByName("Test Cliente");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getTelefono(), "99999999");
}
