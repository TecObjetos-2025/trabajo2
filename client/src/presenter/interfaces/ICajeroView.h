#ifndef ICAJERO_VIEW_H
#define ICAJERO_VIEW_H

#include <vector>
#include <string>
#include "logic/DtoMapper.h"
#include "api/ApiDTOs.h"

class ICajeroView {
public:
    virtual ~ICajeroView() = default;

    virtual void showMenu(const std::vector<ProductViewModel>& products) = 0;
    virtual void showCurrentOrder(const std::vector<OrderItemViewModel>& items) = 0;
    virtual void updateTotals(const std::string& subtotal, const std::string& igv, const std::string& total) = 0;
    virtual void showMessage(const std::string& title, const std::string& msg) = 0;
    virtual void showDiscounts(const std::vector<InfoDescuento>& discounts) = 0;
    virtual void enableFinalizeButton(bool enable) = 0;

    // Inputs from View
    virtual std::string getClientName() = 0;
    virtual std::string getSelectedDiscountId() = 0;

    virtual void clearForm() = 0;
};

#endif // ICAJERO_VIEW_H
