#ifndef COCINA_PRESENTER_H
#define COCINA_PRESENTER_H

#include "presenter/interfaces/ICocinaView.h"
#include "api/ICoreSistema.h"
#include "logic/OrderSorter.h"
#include <sstream>

class CocinaPresenter {
public:
    CocinaPresenter(ICocinaView* view, ICoreSistema* core);

    void refreshKanban();
    void advanceOrder();

private:
    ICocinaView* mView;
    ICoreSistema* mCore;

    std::string formatOrderCard(const InfoPedido& order);
};

#endif // COCINA_PRESENTER_H
