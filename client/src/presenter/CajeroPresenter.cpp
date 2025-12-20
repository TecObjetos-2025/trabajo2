#include "CajeroPresenter.h"
#include <iostream>

CajeroPresenter::CajeroPresenter(ICajeroView* view, ICoreSistema* core)
    : mView(view), mCore(core) {}

void CajeroPresenter::loadInitialData() {
    if (!mCore || !mView) return;

    try {
        // Load Menu
        auto menu = mCore->getMenu();
        std::vector<ProductViewModel> viewMenu;
        for (const auto& prod : menu) {
            viewMenu.push_back(DtoMapper::mapProductToViewModel(prod));
        }
        mView->showMenu(viewMenu);

        // Load Discounts
        auto discounts = mCore->getDescuentosDisponibles();
        mView->showDiscounts(discounts);
    } catch (const std::exception& e) {
        mView->showMessage("Error", std::string("Failed to load data: ") + e.what());
    }
}

void CajeroPresenter::addToOrder(int productId, double price, const std::string& name) {
    mCurrentOrder.push_back({productId, name, price});
    updateOrderView();
    recalculateTotals();
}

void CajeroPresenter::removeFromOrder(int index) {
    if (index >= 0 && index < static_cast<int>(mCurrentOrder.size())) {
        mCurrentOrder.erase(mCurrentOrder.begin() + index);
        updateOrderView();
        recalculateTotals();
    }
}

void CajeroPresenter::updateOrderView() {
    std::vector<OrderItemViewModel> items;
    for (const auto& item : mCurrentOrder) {
        items.push_back(DtoMapper::mapOrderItemToViewModel(item.name, 1, item.price, item.id));
    }
    mView->showCurrentOrder(items);
    mView->enableFinalizeButton(!mCurrentOrder.empty());
}

void CajeroPresenter::recalculateTotals() {
    double subtotal = 0.0;
    for (const auto& item : mCurrentOrder) {
        subtotal += item.price;
    }

    double igvRate = mCore ? mCore->getPorcentajeIGV() : 0.18;
    Totals totals = PriceCalculator::calculate(subtotal, igvRate);

    mView->updateTotals(
        DtoMapper::formatPrice(totals.subtotal),
        DtoMapper::formatPrice(totals.igv),
        DtoMapper::formatPrice(totals.total)
    );
}

void CajeroPresenter::finalizeOrder() {
    std::string client = mView->getClientName();
    if (client.empty()) {
        mView->showMessage("Validation Error", "Please enter the client name.");
        return;
    }

    if (mCurrentOrder.empty()) {
        mView->showMessage("Validation Error", "Order cannot be empty.");
        return;
    }

    // Aggregate items
    std::map<int, int> counts;
    for (const auto& item : mCurrentOrder) {
        counts[item.id]++;
    }

    std::vector<ItemPedidoCrear> itemsVec;
    for (const auto& pair : counts) {
        itemsVec.push_back({pair.first, pair.second});
    }

    std::string discountId = mView->getSelectedDiscountId();

    try {
        mCore->finalizarPedido(client, itemsVec, discountId);
        mView->showMessage("Success", "Order sent successfully.");

        // Clear order and form ONLY on success
        mCurrentOrder.clear();
        updateOrderView();
        recalculateTotals();
        mView->clearForm();
    } catch (const std::exception& e) {
        mView->showMessage("Error", e.what());
    }
}
