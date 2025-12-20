#ifndef DTO_MAPPER_H
#define DTO_MAPPER_H

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "api/ApiDTOs.h"

struct ProductViewModel {
    int id;
    std::string displayName; // "Name - $ Price"
    std::string rawName;
    double price;
};

struct OrderItemViewModel {
    int productId;
    std::string text; // "1x Product Name"
    double price;
};

class DtoMapper {
public:
    static ProductViewModel mapProductToViewModel(const InfoProducto& product);
    static std::string formatPrice(double price);
    static OrderItemViewModel mapOrderItemToViewModel(const std::string& name, int quantity, double price, int id);
};

#endif // DTO_MAPPER_H
