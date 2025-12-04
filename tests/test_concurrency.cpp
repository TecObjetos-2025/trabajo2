#include <gtest/gtest.h>
#include "ColaThreadSafe.h"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

/**
 * @file test_concurrency.cpp
 * @brief Pruebas unitarias para la clase ColaThreadSafe utilizando Google Test.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Este archivo contiene pruebas para verificar el correcto funcionamiento
 * de la clase ColaThreadSafe en escenarios de concurrencia con múltiples hilos.
 */

TEST(ConcurrencyTest, BasicPushPop)
{
    ColaThreadSafe<int> cola;
    cola.push(1);
    cola.push(2);

    EXPECT_EQ(cola.pop(), 1);
    EXPECT_EQ(cola.pop(), 2);
}

TEST(ConcurrencyTest, BlockingPop)
{
    ColaThreadSafe<int> cola;
    std::atomic<bool> popped(false);

    std::thread consumer([&cola, &popped]()
                         {
        int val = cola.pop();
        if (val == 42) popped = true; });

    // Obtener un retardo seguro
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(popped); // Debe estar bloqueado

    cola.push(42);
    consumer.join();

    EXPECT_TRUE(popped);
}

TEST(ConcurrencyTest, MultipleProducersConsumers)
{
    ColaThreadSafe<int> cola;
    const int NUM_THREADS = 4;
    const int ITEMS_PER_THREAD = 1000;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::atomic<int> sum(0);

    // Producers
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        producers.emplace_back([&cola, ITEMS_PER_THREAD]()
                               {
            for (int j = 0; j < ITEMS_PER_THREAD; ++j) {
                cola.push(1);
            } });
    }

    // Consumers
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        consumers.emplace_back([&cola, &sum, ITEMS_PER_THREAD]()
                               {
            for (int j = 0; j < ITEMS_PER_THREAD; ++j) {
                int val = cola.pop();
                sum += val;
            } });
    }

    for (auto &t : producers)
        t.join();
    for (auto &t : consumers)
        t.join();

    int expected_sum = NUM_THREADS * ITEMS_PER_THREAD * 1;
    EXPECT_EQ(sum, expected_sum);
    EXPECT_TRUE(cola.empty());
}

TEST(ConcurrencyTest, CloseQueueReleasesBlock)
{
    ColaThreadSafe<int> cola;
    std::atomic<bool> exception_caught(false);

    std::thread consumer([&cola, &exception_caught]()
                         {
        try {
            cola.pop();
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Cola cerrada") {
                exception_caught = true;
            }
        } });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    cola.cerrar();
    consumer.join();

    EXPECT_TRUE(exception_caught);
}
