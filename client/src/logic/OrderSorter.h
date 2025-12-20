#ifndef ORDER_SORTER_H
#define ORDER_SORTER_H

#include <vector>
#include <string>
#include "api/ApiDTOs.h"

class OrderSorter {
public:
    static std::vector<InfoPedido> filterByStatus(const std::vector<InfoPedido>& orders, const std::string& status);
};

#endif // ORDER_SORTER_H
