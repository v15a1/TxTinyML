#include "Proximity.h"

/// @brief Initializer
Proximity::Proximity()
{
  rssis = new DecisionQueue(40);
  gradients = new DecisionQueue(10);
  inferrer = new Inferrer();
  transferStatus = P_NO_STATUS;
}

/// @brief Enqueues the rssi value to the queue
void Proximity::enqueue(double rssi)
{
  Serial.print("The RSSI is: ");
  Serial.println(rssi);
  if (rssi < 0)
  {
    rssis->enqueue(rssi);
  }
}

/// @brief Invokes inferrer + time estimator to get the final transfer decision.
/// @returns Boolean value of the decision
bool Proximity::getDecision(float battery)
{
  if ((rssis->getCount() < 5) || battery <= 0)
  {
    return false;
  }

  gradientData = gradients->toArray();
  rssiData = rssis->toArray();

  InferrerMetrics confidenceData = getConfidence(battery);
  metrics = confidenceData;
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

/// @brief Calculates the confidence score
/// @returns Inferrer metrics with the confidence
InferrerMetrics Proximity::getConfidence(float battery)
{
  int count = rssis->getCount();

  rssiData = rssis->toArray();
  gradientData = gradients->toArray();

  double lto = contextualizer->getLTO(rssiData);
  gradients->enqueue(lto);
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

/// @brief Estimates the connection time based on the STO
/// @returns estimated connection time
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

unsigned char *Proximity::prepare(int len)
{
  modelSize = len;
  // calculating number of packets to transfer
  unsigned int iters = modelSize / PACKET_SIZE;
  packetCount = iters;
  remModelValueCount = modelSize - (iters * PACKET_SIZE);

  if (remModelValueCount > 0)
  {
    packetCount++;
  }
  // Initializing model chunk holder
  chunks = new unsigned char *[packetCount];
  estimateTimeForModel(len);
}

void Proximity::setModel(unsigned char *model)
{
  if (traversalIndex < modelSize)
  {
    for (int x = 0; x < packetCount; x++)
    {
      chunks[x] = getChunk(model);
      currentIndex++;
    }
  }
}

/// @brief Chunks the model array to transferrable sub-arrays
/// @return Sub-array of the model for transferring
unsigned char *Proximity::getChunk(unsigned char *model)
{
  unsigned char *packetChunk = new unsigned char[PACKET_SIZE + 1]; // sub-array holder
  int startIndex = currentIndex * PACKET_SIZE;
  int checksum = 0;
  for (int j = startIndex; j < startIndex + PACKET_SIZE; j++)
  {
    if (j < modelSize)
    {
      checksum += (int)model[traversalIndex];
      packetChunk[j % PACKET_SIZE] = model[traversalIndex];
    }
    else
    {
      packetChunk[j % PACKET_SIZE] = 0;
    }
    traversalIndex++;
  }
  checksum %= 128;
  packetChunk[PACKET_SIZE] = checksum;
  return packetChunk;
}

void Proximity::reset()
{
  delete chunks;
  delete rssis;
  delete gradients;
  delete inferrer;
}

void Proximity::setStatus(char status)
{
  switch (status)
  {
  case 1:
    transferStatus = C_READY;
    break;
  case 3:
    transferStatus = C_PCKT_COUNT_ACK;
    break;
  case 7:
    transferStatus = C_PCKT_LOSS;
    break;
  case 8:
    transferStatus = C_PCKT_RX;
    break;
  default:
    break;
  }
}

unsigned char Proximity::getStatus()
{
  return transferStatus;
}

int Proximity::getPacketCount()
{
  return packetCount;
}

int16_t Proximity::getTXPcktIndex()
{
  return currentTXPacketIndex;
}

void Proximity::incrementTXPcktIndex()
{
  currentTXPacketIndex++;
}

unsigned char *Proximity::chunkToTransfer()
{
  return chunks[currentTXPacketIndex];
}

InferrerMetrics Proximity::getMetrics()
{
  return metrics;
}
