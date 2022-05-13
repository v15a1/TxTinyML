#include "Contextualizer.h"
#include <Arduino.h>

/// @brief Calculates Optimum/gradient using simple linear regression operators
/// @param data Sequence of normalized RSSI values for calculating the optimum
/// @returns The gradient calculated
double Contextualizer::getLTO(DecisorData data)
{
    // didReceiveNewRSSI = false;
    // int count = data.length;

    // if (count < MAX_ST_RSSI_N) // Checking if the minimum number of samples exist for getting the LTO
    // {
    //     return 0.0;
    // }

    // double score = 0;     // Total RSSI score
    // double scores[count]; // Array to hold the running averages
    // double indices[count];

    // // Setting the scores/running averages for finding the gradients for
    // for (int x = 1; x < count; x++)
    // {
    //     double rssi = data.data[x];
    //     double normRSSI = normalizeRSSI(rssi); // normalizing the RSSI values
    //     score += normRSSI;
    //     double newScore = score / (double)x; // calculating the running average
    //     scores[x] = newScore;
    //     indices[x] = x;
    // }

    // // Structs to hold the necessary data for gradient calculation
    // DecisorData yData;
    // yData.data = scores;
    // yData.length = count;

    // DecisorData xData;
    // xData.data = indices;
    // xData.length = count;

    // /* Calculating the gradient using the simple line equation in linear regression
    //  *
    //  * Calculates the coefficient 'm' in y = mx + c
    //  *
    //  * Gradient is calculated by dividing covariance with the variance calculated using
    //  * provided parameters
    //  */
    // double gradient = calculateGradient(xData, yData);
    // gradients.enqueue(gradient); // Enqueueing the gradient for calculating the STO

    // if (count < MIN_LT_RSSI_N || gradient > 0.01 || gradient < -0.01 || isnan(gradient))
    // {
    //     return 0.0;
    // }
    // return gradient;
}

double Contextualizer::findMean(DecisorData data)
{
    double sum = 0.0;
    for (int x = 0; x < data.length; x++)
    {
        sum += data.data[x];
    }
    double mean = sum / (double)data.length;
    return mean;
}

double Contextualizer::findVariance(DecisorData data, double mean)
{
    double sum = 0.0;
    for (int x = 0; x < data.length; x++)
    {
        double difference = pow((data.data[x] - mean), 2);
        sum += difference;
    }

    return sum;
}

double Contextualizer::findCovariance(DecisorData dataX, double meanX, DecisorData dataY, double meanY)
{
    double covar = 0.0;
    int length = dataX.length;

    for (int x = 0; x < length; x++)
    {
        double op1 = (dataX.data[x] - meanX);
        double op2 = (dataY.data[x] - meanY);
        covar += op1 * op2;
    }
    return covar;
}

double Contextualizer::calculateGradient(DecisorData dataX, DecisorData dataY)
{
    double meanX = findMean(dataX);
    double meanY = findMean(dataY);

    double covariance = findCovariance(dataX, meanX, dataY, meanY);
    double variance = findVariance(dataX, meanX);
    return covariance / variance;
}

double Contextualizer::calculateCoeff(DecisorData dataX, double x_mean, DecisorData dataY, double y_mean)
{
    double b1 = calculateGradient(dataX, dataY);
    double b0 = y_mean - b1 * x_mean;
    return b0;
}

double Contextualizer::normalizeOptimality(double value, double max, double min)
{
    if (isnan(value) || value == 0.0)
    {
        return 0.0;
    }

    double normValue = ((value - (min)) / (max - (min))) * 100;
    return truncate(normValue, 6);
}

double Contextualizer::truncate(double val, unsigned char dec)
{
    double x = val * pow(10, dec);
    double y = round(x);
    double z = x - y;
    if ((int)z == 5)
    {
        y++;
    }
    else
    {
    }
    x = y / pow(10, dec);
    return x;
}
