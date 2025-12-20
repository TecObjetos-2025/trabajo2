#ifndef PRICE_CALCULATOR_H
#define PRICE_CALCULATOR_H

struct Totals {
    double subtotal;
    double igv;
    double total;
};

class PriceCalculator {
public:
    static Totals calculate(double subtotal, double igvRate);
};

#endif // PRICE_CALCULATOR_H
