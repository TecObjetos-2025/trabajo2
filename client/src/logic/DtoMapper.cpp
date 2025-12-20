#include "DtoMapper.h"

std::string DtoMapper::formatPrice(double price) {
    std::stringstream ss;
    ss << "$ " << std::fixed << std::setprecision(2) << price;
    return ss.str();
}

ProductViewModel DtoMapper::mapProductToViewModel(const InfoProducto& product) {
    std::stringstream ss;
    ss << product.nombre << " - S/ " << std::fixed << std::setprecision(2) << product.precio;
    return {product.id, ss.str(), product.nombre, product.precio};
}

OrderItemViewModel DtoMapper::mapOrderItemToViewModel(const std::string& name, int quantity, double price, int id) {
    std::stringstream ss;
    ss << quantity << "x " << name;
    return {id, ss.str(), price};
}
