#include "PriceCalculator.h"

Totals PriceCalculator::calculate(double subtotal, double igvRate) {
    double igv = subtotal * igvRate;
    double total = subtotal + igv;
    return {subtotal, igv, total};
}
