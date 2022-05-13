#ifndef PROXIMITY_CONTEXTUALIZER_H_
#define PROXIMITY_CONTEXTUALIZER_H_

#include "DecisorData.h"

class Contextualizer
{
public:
    double getLTO(DecisorData data);
    double getSTO(DecisorData data);
    double normalizeOptimality(double value, double max, double min);
    double truncate(double val, unsigned char dec);

private:
    double calculateGradient(DecisorData dataX, DecisorData dataY);
    double calculateCoeff(DecisorData dataX, double x_mean, DecisorData dataY, double y_mean);
    double findCovariance(DecisorData dataX, double meanX, DecisorData dataY, double meanY);
    double findVariance(DecisorData data, double mean);
    double findMean(DecisorData data);
};
#endif