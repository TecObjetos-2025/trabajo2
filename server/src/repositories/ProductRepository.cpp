#include "repositories/ProductRepository.h"
#include "db/DatabaseManager.h"
#include <iostream>

ProductRepository::ProductRepository() {}

void ProductRepository::loadFromDB()
{
    productCache.clear();
    auto productosDTO = DatabaseManager::instance().getProductos();
    for (const auto &dto : productosDTO)
    {
        // Category is empty for now as per previous logic
        auto prod = std::make_shared<Producto>(dto.id, dto.nombre, dto.precio, "");
        productCache[dto.id] = prod;
    }
    std::cout << "[ProductRepository] Loaded " << productCache.size() << " products from DB." << std::endl;
}

std::shared_ptr<Producto> ProductRepository::findById(int id) const
{
    auto it = productCache.find(id);
    if (it != productCache.end())
    {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Producto>> ProductRepository::getAll() const
{
    std::vector<std::shared_ptr<Producto>> allProducts;
    allProducts.reserve(productCache.size());
    for (const auto &pair : productCache)
    {
        allProducts.push_back(pair.second);
    }
    return allProducts;
}

void ProductRepository::add(std::shared_ptr<Producto> product)
{
    if (product)
    {
        productCache[product->getId()] = product;
    }
}
