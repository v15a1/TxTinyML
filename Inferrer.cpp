#include "Inferrer.h"
#include <Fuzzy.h>

Inferrer::Inferrer()
{
    // Fuzzy inferrer initializer
    fuzzy = new Fuzzy();
    // Initializing Membership functions
    // Battery Membership Functions
    xLowBattery = new FuzzySet(0, 0, 0, 10);
    lowBattery = new FuzzySet(5, 25, 25, 50);
    avgBattery = new FuzzySet(20, 60, 60, 80);
    highBattery = new FuzzySet(50, 100, 100, 100);
    // STO Membership Functions
    lowSto = new FuzzySet(0, 25, 25, 50);
    highSto = new FuzzySet(50, 75, 75, 100);
    // LTO Membership Functions
    lowLto = new FuzzySet(0, 25, 25, 50);
    highLto = new FuzzySet(50, 75, 75, 100);
    // Output Membership functions
    tx_hold = new FuzzySet(0, 0, 0, 50);
    tx_send = new FuzzySet(50, 100, 100, 100);

    // Setup
    // Adding fuzzy sets/membership functions for each input
    FuzzyInput *battery = new FuzzyInput(1);
    battery->addFuzzySet(xLowBattery);
    battery->addFuzzySet(lowBattery);
    battery->addFuzzySet(avgBattery);
    battery->addFuzzySet(highBattery);
    fuzzy->addFuzzyInput(battery);

    FuzzyInput *sto = new FuzzyInput(2);
    sto->addFuzzySet(lowSto);
    sto->addFuzzySet(highSto);
    fuzzy->addFuzzyInput(sto);

    FuzzyInput *lto = new FuzzyInput(3);
    lto->addFuzzySet(lowLto);
    lto->addFuzzySet(highLto);
    fuzzy->addFuzzyInput(lto);

    FuzzyOutput *transfer = new FuzzyOutput(1);
    transfer->addFuzzySet(tx_hold);
    transfer->addFuzzySet(tx_send);
    fuzzy->addFuzzyOutput(transfer);

    setupTransferRules();
    setupHoldRules();
}

/// @brief Sets up the rules for transferring a model
void Inferrer::setupTransferRules()
{
    /*
     * Setup of Transfer Rule 1:
     * Battery is extremely-low AND if long term optimum is high
     */
    FuzzyRuleAntecedent *highLtoRule = new FuzzyRuleAntecedent();
    highLtoRule->joinSingle(highLto);

    FuzzyRuleAntecedent *batteryIsXLow = new FuzzyRuleAntecedent();
    batteryIsXLow->joinSingle(xLowBattery);

    FuzzyRuleAntecedent *firstTXRule = new FuzzyRuleAntecedent();
    firstTXRule->joinWithAND(highLtoRule, batteryIsXLow);

    FuzzyRuleConsequent *firstTXRuleSatisfied = new FuzzyRuleConsequent();
    firstTXRuleSatisfied->addOutput(tx_send);

    FuzzyRule *fuzzyRule1 = new FuzzyRule(1, firstTXRule, firstTXRuleSatisfied);
    fuzzy->addFuzzyRule(fuzzyRule1);

    /*
     * Setup of Transfer Rule 2:
     * Battery is NOT extremely-low AND if short term AND long term optimums are high
     */
    FuzzyRuleAntecedent *batteryLowOrAvg = new FuzzyRuleAntecedent();
    batteryLowOrAvg->joinWithOR(lowBattery, avgBattery);

    FuzzyRuleAntecedent *batteryLowOrAvgOrHigh = new FuzzyRuleAntecedent();
    batteryLowOrAvgOrHigh->joinWithOR(batteryLowOrAvg, highBattery);

    FuzzyRuleAntecedent *oIsAvgOrHigh = new FuzzyRuleAntecedent();
    oIsAvgOrHigh->joinWithOR(highSto, highLto);

    FuzzyRuleAntecedent *secondTXRule = new FuzzyRuleAntecedent();
    secondTXRule->joinWithAND(oIsAvgOrHigh, batteryLowOrAvgOrHigh);

    FuzzyRuleConsequent *secondTXRuleSatisfied = new FuzzyRuleConsequent();
    secondTXRuleSatisfied->addOutput(tx_send);

    FuzzyRule *fuzzyRule2 = new FuzzyRule(2, secondTXRule, secondTXRuleSatisfied);
    fuzzy->addFuzzyRule(fuzzyRule2);
}

/// @brief Sets up the rules for holding a model transfer
void Inferrer::setupHoldRules()
{
    /*
     * Setup of Hold Rule 1:
     * Battery is NOT extremely-low AND if long term AND short term optimums are NOT high
     */
    FuzzyRuleAntecedent *batteryIsAvgOrHigh = new FuzzyRuleAntecedent();
    batteryIsAvgOrHigh->joinWithOR(highBattery, avgBattery);

    FuzzyRuleAntecedent *batteryIsLowOrAvgOrHigh = new FuzzyRuleAntecedent();
    batteryIsLowOrAvgOrHigh->joinWithOR(batteryIsAvgOrHigh, lowBattery);

    FuzzyRuleAntecedent *ltoIsLow = new FuzzyRuleAntecedent();
    ltoIsLow->joinSingle(lowLto);

    FuzzyRuleAntecedent *stoIsLow = new FuzzyRuleAntecedent();
    stoIsLow->joinSingle(lowSto);

    FuzzyRuleAntecedent *stoAndLtoIsLow = new FuzzyRuleAntecedent();
    batteryIsAvgOrHigh->joinWithAND(stoIsLow, ltoIsLow);

    FuzzyRuleAntecedent *stoAndLtoIsLowAndBattNotXLow = new FuzzyRuleAntecedent();
    stoAndLtoIsLowAndBattNotXLow->joinWithAND(stoAndLtoIsLow, batteryIsLowOrAvgOrHigh);

    FuzzyRuleConsequent *firstHoldRuleSatisfied = new FuzzyRuleConsequent();
    firstHoldRuleSatisfied->addOutput(tx_hold);

    FuzzyRule *fuzzyRule4 = new FuzzyRule(3, stoAndLtoIsLowAndBattNotXLow, firstHoldRuleSatisfied);
    fuzzy->addFuzzyRule(fuzzyRule4);
}

/// @brief Runs inference using the fuzzy logic inferrer
/// @param lto-normalized LTO value
/// @param sto-normalized STO value
/// @param battery-current battery level
float Inferrer::infer(float lto, float sto, float batteryLevel)
{
    fuzzy->setInput(1, batteryLevel);
    fuzzy->setInput(2, sto);
    fuzzy->setInput(3, lto);

    fuzzy->fuzzify();

    float output1 = fuzzy->defuzzify(1);
    return output1;
}
