#include "OrderSorter.h"

std::vector<InfoPedido> OrderSorter::filterByStatus(const std::vector<InfoPedido>& orders, const std::string& status) {
    std::vector<InfoPedido> filtered;
    for (const auto& order : orders) {
        if (order.estado == status) {
            filtered.push_back(order);
        }
    }
    return filtered;
}
