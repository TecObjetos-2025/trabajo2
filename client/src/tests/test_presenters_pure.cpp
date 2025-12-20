#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <memory>
#include "../presenter/CajeroPresenter.h"
#include "../presenter/CocinaPresenter.h"
#include "../../common/include/api/ICoreSistema.h"
#include "../presenter/interfaces/ICajeroView.h"
#include "../presenter/interfaces/ICocinaView.h"

// --- Mocks ---

class MockCore : public ICoreSistema {
public:
    void registrarObservador(std::shared_ptr<IObservadorCore> observador) override {}
    void removerObservador(IObservadorCore *observador) override {}

    std::vector<InfoProducto> getMenu() override {
        return {
            {1, "Cafe", 5.00},
            {2, "Sandwich", 10.00}
        };
    }

    std::vector<InfoDescuento> getDescuentosDisponibles() override {
        return {{"NONE", "Sin descuento"}};
    }

    double getPorcentajeIGV() const override { return 0.18; }

    void finalizarPedido(const std::string &cliente,
                         const std::vector<ItemPedidoCrear> &items,
                         const std::string &id_descuentos) override {
        lastOrderClient = cliente;
        lastOrderItems = items;
    }

    std::vector<InfoPedido> getPedidosActivos() override {
        return {
            {101, "Alice", "En Cola", {{"Cafe", 1, 5.00}}, 5.00},
            {102, "Bob", "Listo", {{"Sandwich", 1, 10.00}}, 10.00}
        };
    }

    void procesarSiguientePedido() override {
        processedNext = true;
    }

    // Test verification helpers
    std::string lastOrderClient;
    std::vector<ItemPedidoCrear> lastOrderItems;
    bool processedNext = false;
};

class MockCajeroView : public ICajeroView {
public:
    void showMenu(const std::vector<ProductViewModel>& products) override {}
    void showCurrentOrder(const std::vector<OrderItemViewModel>& items) override {
        currentItems = items;
    }
    void updateTotals(const std::string& subtotal, const std::string& igv, const std::string& total) override {
        lastSubtotal = subtotal;
        lastIGV = igv;
        lastTotal = total;
    }
    void showMessage(const std::string& title, const std::string& msg) override {
        lastMessage = msg;
    }
    void showDiscounts(const std::vector<InfoDescuento>& discounts) override {}
    void enableFinalizeButton(bool enable) override {}

    std::string getClientName() override { return "Test Client"; }
    std::string getSelectedDiscountId() override { return "NONE"; }

    void clearForm() override {
        formCleared = true;
    }

    // Test helpers
    bool formCleared = false;
    std::vector<OrderItemViewModel> currentItems;
    std::string lastSubtotal;
    std::string lastIGV;
    std::string lastTotal;
    std::string lastMessage;
};

class MockCocinaView : public ICocinaView {
public:
    void clearKanbanBoard() override {
        columns.clear();
    }

    void addOrderToColumn(const std::string& status, const std::string& cardContent) override {
        columns[status].push_back(cardContent);
    }

    void showKitchenMessage(const std::string& msg) override {
        lastMessage = msg;
    }

    // Test helpers
    std::map<std::string, std::vector<std::string>> columns;
    std::string lastMessage;
};

// --- Tests ---

void test_CajeroLogic() {
    MockCore core;
    MockCajeroView view;
    CajeroPresenter presenter(&view, &core);

    // 1. Add item
    presenter.addToOrder(1, 5.00, "Cafe");

    assert(view.currentItems.size() == 1);
    assert(view.currentItems[0].text == "1x Cafe");
    // 5.00 subtotal, 0.90 IGV, 5.90 Total
    assert(view.lastSubtotal == "$ 5.00");
    assert(view.lastIGV == "$ 0.90");
    assert(view.lastTotal == "$ 5.90");

    // 2. Add another item
    presenter.addToOrder(2, 10.00, "Sandwich");
    assert(view.currentItems.size() == 2);
    // 15.00 subtotal, 2.70 IGV, 17.70 Total
    assert(view.lastSubtotal == "$ 15.00");
    assert(view.lastIGV == "$ 2.70");
    assert(view.lastTotal == "$ 17.70");

    // 3. Finalize
    presenter.finalizeOrder();
    assert(view.lastMessage == "Order sent successfully.");
    assert(core.lastOrderClient == "Test Client");
    assert(core.lastOrderItems.size() == 2);
    assert(view.formCleared == true);

    std::cout << "test_CajeroLogic PASSED" << std::endl;
}

void test_CocinaLogic() {
    MockCore core;
    MockCocinaView view;
    CocinaPresenter presenter(&view, &core);

    // 1. Refresh Board
    presenter.refreshKanban();
    assert(view.columns["En Cola"].size() == 1);
    assert(view.columns["Listo"].size() == 1);

    // 2. Advance Order
    presenter.advanceOrder();
    assert(core.processedNext == true);
    assert(view.lastMessage == "Next order processed.");

    std::cout << "test_CocinaLogic PASSED" << std::endl;
}

int main() {
    test_CajeroLogic();
    test_CocinaLogic();
    return 0;
}
