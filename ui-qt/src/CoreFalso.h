#ifndef CORE_FALSO_H
#define CORE_FALSO_H

#include "api/ICoreSistema.h"
#include "api/ApiDTOs.h"
#include <vector>
#include <iostream>

class CoreFalso : public ICoreSistema
{
private:
    std::vector<IObservadorCore *> observadores;

public:
    CoreFalso();
    virtual ~CoreFalso() = default;

    // --- Implementacionn de ICoreSistema

    // Gestion de Observadores
    virtual void registrarObservador(IObservadorCore *observador) override;
    virtual void removerObservador(IObservadorCore *observador) override;

    // Funciones "Vista Cajero"
    virtual std::vector<InfoProducto> getMenu() override;
    virtual std::vector<InfoDescuento> getDescuentosDisponibles() override;
    virtual void finalizarPedido(const std::string &cliente,
                                 const std::vector<ItemPedidoCrear> &items,
                                 const std::string &id_descuentos) override;

    // Funciones "Vista Cocina"
    virtual std::vector<InfoPedido> getPedidosEnCola() override;
    virtual void procesarSiguientePedido() override;
};

#endif // CORE_FALSO_H