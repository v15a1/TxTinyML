#include "Contextualizer.h"
#include <Arduino.h>

/// @brief Calculates Optimum/gradient using simple linear regression operators
/// @param data Sequence of normalized RSSI values for calculating the optimum
/// @returns The gradient calculated
double Contextualizer::getLTO(DecisorData data)
{
    int count = data.length;

    if (count < 8) // Checking if the minimum number of samples exist for getting the LTO
    {
        return 0.0;
    }

    double score = 0;     // Total RSSI score
    double scores[count]; // Array to hold the running averages
    double indices[count];

    // Setting the scores/running averages for finding the gradients for
    for (int x = 1; x < count; x++)
    {
        double rssi = data.data[x];
        double normRSSI = normalizeRSSI(rssi); // normalizing the RSSI values
        score += normRSSI;
        double newScore = score / (double)x; // calculating the running average
        scores[x] = newScore;
        indices[x] = x;
    }

    // Structs to hold the necessary data for gradient calculation
    DecisorData yData;
    yData.data = scores;
    yData.length = count;

    DecisorData xData;
    xData.data = indices;
    xData.length = count;

    /* Calculating the gradient using the simple line equation in linear regression
     *
     * Calculates the coefficient 'm' in y = mx + c
     *
     * Gradient is calculated by dividing covariance with the variance calculated using
     * provided parameters
     */
    double gradient = calculateGradient(xData, yData);

    if (count < 15 || gradient > 0.01 || gradient < -0.01 || isnan(gradient))
    {
        return 0.0;
    }
    return gradient;
}

/// @brief Calculates the average gradient change rate for checking if it's ideal for a
/// transfer in the past few seconds
/// @param gradients Sequence of previously calculated gradients/LTOs for calculating the STO
/// @returns The average gradient change rate.
double Contextualizer::getSTO(DecisorData gradients)
{
    double rateSum = 0;
    int count = gradients.length - 1;
    for (int x = 0; x < count; x++)
    {
        // Calculating the rate upto x + 1 elements
        double rate = gradients.data[x + 1] - gradients.data[x];
        rate = truncate(rate, 5);
        // Checking if value is nan
        if (!isnan(rate))
        {
            rateSum += rate; // Incrementing the rate sum for final averaging
        }
        else
        {
            count--; // Reducing the number of iterations if a value is nan
        }
    }
    // The average gradient change rate is averaged and returned
    double sto = rateSum / (double)count;
    return sto; // Returning the STO
}

/// Calculating the gradient
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

/// @brief Normalizes the optimalities
double Contextualizer::normalizeOptimality(double value, double max, double min)
{
    if (isnan(value) || value == 0.0)
    {
        return 0.0;
    }

    double normValue = ((value - (min)) / (max - (min))) * 100;
    return truncate(normValue, 6);
}

/// @brief truncates a double to a fixed number of decimal places
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

/// @brief Normalizes the RSSI
double Contextualizer::normalizeRSSI(double rssi)
{
    double numerator = rssi - (-100.0);
    double denomenator = 0 - (-100.0);
    return truncate(numerator / denomenator, 6);
}