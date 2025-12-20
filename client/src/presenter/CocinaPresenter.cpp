#include "CocinaPresenter.h"

CocinaPresenter::CocinaPresenter(ICocinaView* view, ICoreSistema* core)
    : mView(view), mCore(core) {}

void CocinaPresenter::refreshKanban() {
    if (!mCore || !mView) return;

    try {
        auto orders = mCore->getPedidosActivos();
        mView->clearKanbanBoard();

        for (const auto& order : orders) {
            std::string cardContent = formatOrderCard(order);
            mView->addOrderToColumn(order.estado, cardContent);
        }
    } catch (const std::exception& e) {
        mView->showKitchenMessage(std::string("Error refreshing board: ") + e.what());
    }
}

std::string CocinaPresenter::formatOrderCard(const InfoPedido& order) {
    std::stringstream ss;
    ss << "Pedido #" << order.id_pedido << "\n";
    ss << "Cliente: " << order.cliente << "\n";
    for (const auto& item : order.items) {
        ss << "\n• " << item.nombreProducto << " x" << item.cantidad;
    }
    return ss.str();
}

void CocinaPresenter::advanceOrder() {
    if (!mCore) return;
    try {
        mCore->procesarSiguientePedido();
        mView->showKitchenMessage("Next order processed.");
    } catch (const std::exception& e) {
        mView->showKitchenMessage(e.what());
    }
}
