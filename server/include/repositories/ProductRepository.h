#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "models/Producto.h"

class ProductRepository
{
public:
    ProductRepository();
    ~ProductRepository() = default;

    /**
     * @brief Loads products from the database into the cache.
     */
    void loadFromDB();

    /**
     * @brief Finds a product by its ID.
     * @param id The product ID.
     * @return Shared pointer to the product, or nullptr if not found.
     */
    std::shared_ptr<Producto> findById(int id) const;

    /**
     * @brief Retrieves all products currently in the cache.
     * @return A vector of all products.
     */
    std::vector<std::shared_ptr<Producto>> getAll() const;

    /**
     * @brief Adds a product to the repository (cache).
     * @param product The product to add.
     */
    void add(std::shared_ptr<Producto> product);

private:
    std::unordered_map<int, std::shared_ptr<Producto>> productCache;
};
