
#ifndef PROXIMITY_INFERRER_H_
#define PROXIMITY_INFERRER_H_

#include <Fuzzy.h>

class Inferrer
{
public:
    Inferrer();
    float infer(float lto, float sto, float batteryLevel);

private:
    Fuzzy *fuzzy;

    FuzzySet *xLowBattery;
    FuzzySet *lowBattery;
    FuzzySet *avgBattery;
    FuzzySet *highBattery;

    FuzzySet *lowSto;
    FuzzySet *avgSto;
    FuzzySet *highSto;

    FuzzySet *lowLto;
    FuzzySet *avgLto;
    FuzzySet *highLto;

    FuzzySet *tx_hold;
    FuzzySet *tx_potential;
    FuzzySet *tx_send;

    void setupTransferRules();
    void setupHoldRules();
};
#endif