#ifndef CAJERO_PRESENTER_H
#define CAJERO_PRESENTER_H

#include <vector>
#include <memory>
#include <map>
#include "presenter/interfaces/ICajeroView.h"
#include "api/ICoreSistema.h"
#include "logic/PriceCalculator.h"
#include "logic/DtoMapper.h"

class CajeroPresenter {
public:
    CajeroPresenter(ICajeroView* view, ICoreSistema* core);

    void loadInitialData();
    void addToOrder(int productId, double price, const std::string& name);
    void removeFromOrder(int index);
    void finalizeOrder();

private:
    ICajeroView* mView;
    ICoreSistema* mCore;

    // Internal state for the current order
    // We store items as they are added to the list
    struct OrderItem {
        int id;
        std::string name;
        double price;
    };
    std::vector<OrderItem> mCurrentOrder;

    void recalculateTotals();
    void updateOrderView();
};

#endif // CAJERO_PRESENTER_H
