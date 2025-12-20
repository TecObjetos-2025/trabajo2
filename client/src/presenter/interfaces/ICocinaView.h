#ifndef ICOCINA_VIEW_H
#define ICOCINA_VIEW_H

#include <string>

class ICocinaView {
public:
    virtual ~ICocinaView() = default;

    virtual void clearKanbanBoard() = 0;

    // status: "En Cola", "En Preparación", "Listo"
    virtual void addOrderToColumn(const std::string& status, const std::string& cardContent) = 0;

    virtual void showKitchenMessage(const std::string& msg) = 0;
};

#endif // ICOCINA_VIEW_H
