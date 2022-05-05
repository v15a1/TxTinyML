#ifndef PROXIMITY_H_
#define PROXIMITY_H_

#include "DecisionQueue.h"
#include "DecisorData.h"

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

public:
    Proximity();
    void reset();
    float getConfidence(int rssi, float battery);
    float estimateTxTime(float modelSize);
    float normalizeOptimalities(float value);
};
#endif
