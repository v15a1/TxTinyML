#include "Proximity.h"

Proximity::Proximity()
{
  rssis = new DecisionQueue(40);
  gradients = new DecisionQueue(10);
  inferrer = new Inferrer();
}

/// @brief Invokes inferrer + time estimator to get the final transfer decision.
bool Proximity::getDecision(float battery)
{
  if ((rssis->getCount() < 5) || battery <= 0)
  {
    return false;
  }

  gradientData = gradients->toArray();
  rssiData = rssis->toArray();

  // TODO: CAST battery to Double

  InferrerMetrics confidenceData = getConfidence(battery);
  if (confidenceData.confidence > 80)
  {
    double estTime = estimateConnectionTime(confidenceData.sto);
    estimatedTime = 0;
    if (estimatedModelTXTime < estTime)
    {
      return true;
    }
    return false;
  }
  else
  {
    return false;
  }
}

InferrerMetrics Proximity::getConfidence(float battery)
{
  int count = rssis->getCount();

  rssiData = rssis->toArray();
  gradientData = gradients->toArray();

  double lto = contextualizer->getLTO(rssiData);
  double sto = contextualizer->getSTO(gradientData);

  InferrerMetrics metrics;
  metrics.battery = battery;
  metrics.sto = sto;
  metrics.lto = lto;

  double stoNorm = contextualizer->normalizeOptimality(sto, 0.005, -0.005);
  double ltoNorm = contextualizer->normalizeOptimality(lto, 0.01, -0.01);

  double confidence = inferrer->infer(ltoNorm, stoNorm, battery);
  metrics.confidence = confidence;

  return metrics;
}

/// @brief Estimates the connection time based on th
double Proximity::estimateConnectionTime(double sto)
{
  if (sto > 0)
  {
    getTimeEstForGradient(sto, 0.5, 0.5);
    return contextualizer->truncate((estimatedTime * 2), 4);
  }
  return 0;
}

double Proximity::getTimeEstForGradient(double sto, double refreshT, double exponent)
{
  // Serial.print("STO : ");
  // Serial.println(sto, 10);
  double sqrt = pow(sto, exponent);
  double time = 0;
  if (sqrt > 0.00 && sqrt < 0.9999)
  {
    estimatedTime += refreshT;
    double iterativeT = getTimeEstForGradient(sqrt, refreshT, exponent);
    estimatedTime += iterativeT;
  }
  return time;
}

void Proximity::estimateTimeForModel(int modelSize)
{
  double estTXTime = packetCount * AVG_PACKET_TX_TIME;
  estimatedModelTXTime = estTXTime;
}