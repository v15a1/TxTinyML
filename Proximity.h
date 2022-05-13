#ifndef PROXIMITY_H_
#define PROXIMITY_H_

#include <Arduino.h>
#include "DecisionQueue.h"
#include "DecisorData.h"
#include "Inferrer.h"
#include "InferrerMetrics.h"
#include "Contextualizer.h"

#define PACKET_SIZE 128
#define AVG_PACKET_TX_TIME 0.165781 // <- Average time for a packet transfer
#define RSSI_REFRESH_TIME 200

/// @brief Statuses of the transfer process
enum TransferStatus
{
    P_NO_STATUS = 0x00,
    C_READY = 0x01,
    P_NOTIFY_PCKT_COUNT = 0x02,
    C_PCKT_COUNT_ACK = 0x03,
    P_PREPARE_TX = 0x04,
    P_TRANSFERRING = 0x05,
    P_PCKT_TX = 0x06,
    C_PCKT_LOSS = 0x07,
    C_PCKT_RX = 0x08,
    P_TX_COMPLETE = 0x09,
};

class Proximity
{
private:
    // Queues for keeping track of the RSSI's and gradients
    DecisionQueue *rssis;
    DecisionQueue *gradients;
    // Stored data for decision making
    DecisorData rssiData;
    DecisorData gradientData;
    // Inferrer
    Inferrer *inferrer;
    Contextualizer *contextualizer;
    // Data preparation variables
    int traversalIndex = 0;
    int currentIndex = 0;
    int modelSize;
    unsigned int remModelValueCount; // Holds the number of remaining model values (bytes)

    double estimatedModelTXTime;
    double estimatedTime = 0;
    int packetCount;        // TODO: Set the packet count
    unsigned char **chunks; // Holds the model in a 2D array, where each sub-array is a chunk for transmission
    unsigned char transferStatus;
    int16_t currentTXPacketIndex = 0;

    InferrerMetrics metrics;

    InferrerMetrics
    getConfidence(float battery);
    void estimateTimeForModel(int modelSize);
    double estimateConnectionTime(double sto);
    double getTimeEstForGradient(double sto, double refreshT, double exponent);
    unsigned char *getChunk(unsigned char *model);

public:
    Proximity();
    void enqueue(double rssi);
    float estimateTxTime(float modelSize);
    float normalizeOptimalities(float value);
    bool getDecision(float battery);
    unsigned char *prepare(int modelLength);
    void setModel(unsigned char *model);
    void setStatus(char status);
    unsigned char getStatus();
    int getPacketCount();
    int16_t getTXPcktIndex();
    void incrementTXPcktIndex();
    unsigned char *chunkToTransfer();
    void reset();
    InferrerMetrics getMetrics();
};
#endif
