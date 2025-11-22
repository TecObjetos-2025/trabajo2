#ifndef COLA_THREAD_SAFE_H
#define COLA_THREAD_SAFE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>

// Monitor thread-safe para cola de pedidos (o cualquier tipo)
template <typename T>
class ColaThreadSafe
{
private:
    std::deque<T> cola;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    ColaThreadSafe() = default;
    ~ColaThreadSafe() = default;

    // Productor: agrega elemento al final
    void push(const T &elemento)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            cola.push_back(elemento);
        }
        cv.notify_one();
    }

    // Consumidor: obtiene y elimina el elemento del frente, espera si está vacía
    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !cola.empty(); });
        T elemento = cola.front();
        cola.pop_front();
        return elemento;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return cola.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return cola.size();
    }

    // Snapshot de elementos para depuración
    std::deque<T> snapshot() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return cola;
    }
};

#endif // COLA_THREAD_SAFE_H
