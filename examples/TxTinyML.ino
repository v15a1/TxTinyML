#include <ArduinoBLE.h>
#include <Proximity.h>
#include <TimedAction.h>

#define PACKET_SIZE 128       // Maximum payload
#define RSSI_REFRESH_TIME 200 // Millis

BLEDevice centralDevice;
BLEService transferService("03CCB8A4-0C70-4D63-8EBE-E8F6FA339FB7");

// Characteristics used for transferring
BLEUnsignedCharCharacteristic transferStatusChar("2cfca732-7f87-4e54-a00f-a7be43d6bf49", BLERead | BLEWrite | BLEWriteWithoutResponse);
BLEIntCharacteristic transferPacketCountChar("9d4a4fc1-8eeb-4785-859b-32c584a46f5e", BLERead | BLENotify);
BLECharacteristic packetTransferChar("5b612497-2af9-46c8-b532-48bd186035f3", BLERead | BLENotify, (PACKET_SIZE + 1));

Proximity *proximity = new Proximity();

bool isTransferring = false;
bool didReceiveNewRSSI = false;
float battery = 25;

unsigned char transferStatus = P_NO_STATUS; // Status of the transfer process

/// @brief Proto-thread callback
void readRSSI()
{
  if (centralDevice)
  {
    double rssi = (double)centralDevice.rssi();
    // Serial.println(rssi);
    proximity->enqueue(rssi);
    didReceiveNewRSSI = true;
  }
}
// Prot-thread initializer
TimedAction rssiReadThread = TimedAction(RSSI_REFRESH_TIME, readRSSI);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  if (!BLE.begin())
  {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  setupCharacteristics();
  Serial.println("Prepared");
  // proximity->prepare(2048);
}

void loop()
{
  centralDevice = BLE.central();
  // Checking if central exists
  if (centralDevice)
  {
    Serial.print("Connected to central: ");
    Serial.println(centralDevice.address());
    // connecting to central
    while (centralDevice.connected())
    {
      digitalWrite(LED_BUILTIN, HIGH);
      rssiReadThread.check(); // Firing proto-thread if necessary
      BLE.poll();             // Checking if any characteristics have changes
      InferrerMetrics m = proximity->getMetrics();
      Serial.print("confidence: ");
      Serial.println(m.confidence);
      if (didReceiveNewRSSI && !isTransferring)
      {
        isTransferring = proximity->getDecision(battery); // Receiving transfer decision
        didReceiveNewRSSI = false;
      }
      else if (isTransferring)
      {
        Serial.println("Transferring...");
        while (isTransferring)
        {
          BLE.poll();
          transferIfNeeded(); // Transferring the data
          battery -= 0.000002;
        }
      }
    }
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    Serial.println("Doing other tasks!");
  }
}

/// @brief Sets up the characteristics that are needed for transferring the data
void setupCharacteristics()
{
  BLE.setLocalName("Proximity");
  BLE.setAdvertisedService(transferService); // add the service UUID

  transferService.addCharacteristic(transferStatusChar);      // Adding the characteristic responsible for notifying statuses
  transferService.addCharacteristic(transferPacketCountChar); // Adding the characteristic responsible for notifying central of the packet counts
  transferService.addCharacteristic(packetTransferChar);      // Adding the characteristic responsible for packet transfers

  BLE.addService(transferService); // Add the transfer service

  transferStatusChar.writeValue(P_NO_STATUS);                            // set initial value for the status characteristic
  transferStatusChar.setEventHandler(BLEWritten, checkForStatusUpdates); // Event handler

  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  BLE.advertise();
}

/// @brief Transfers the model based on the status
void transferIfNeeded()
{
  // Running functionality based on the status
  switch (proximity->getStatus())
  {
  case C_READY:
    // TODO: Uncomment and add model
    //  proximity->setModel();
    // transferPacketCountChar.writeValue(modelLength);
    transferStatus = P_NOTIFY_PCKT_COUNT;
    break;

  case C_PCKT_COUNT_ACK:
    transferStatusChar.writeValue(P_PREPARE_TX); // notifying central that the device is preparing the model for transmission
    // TODO: Uncomment
    //  proximity->setModel(/*MODEL*/);
    transferStatus = P_TRANSFERRING;
    break;

  case P_TRANSFERRING:
    transferPackets();
    transferStatusChar.writeValue(P_TX_COMPLETE);

    BLE.stopAdvertise();
    centralDevice.disconnect();
  default:
    break;
  }
}

/// @brief Checks for updates from a central for a specific characteristic and sets the new status
/// @param central BLE Central
/// @param characteristic BLECharacterisitic to read values from
void checkForStatusUpdates(BLEDevice central, BLECharacteristic characteristic)
{
  proximity->setStatus(transferStatusChar.value());
}

///@brief Disconnect handler
void blePeripheralDisconnectHandler(BLEDevice central)
{
  // endTime = millis();
  // // central disconnected event handler
  // Serial.print("Disconnecteded from central: ");
  // Serial.println(central.address());
  // if (isTransferring)
  // {
  //   Serial.print("Total time for transfer: ");
  //   Serial.println(endTime - startTime);
  //   Serial.print("Time for Decision making: ");
  //   Serial.println(decisionTime);
  //   Serial.print("Time for Transferring: ");
  //   Serial.println(endTime - transferTime);
  // }
  // else
  // {
  //   Serial.println("Did NOT transfer");
  // }

  // transferStatus = P_NO_STATUS;
  // isTransferring = false;
  // didReceiveNewRSSI = false;

  // battery = 23.0;
  // estimatedModelTXTime;
  // estimatedTime = 0;
}

/// @brief Updates chunk char and awaits ack from the central device
void transferPackets()
{
  sendChunkAwaitAck();
  while (proximity->getTXPcktIndex() < proximity->getPacketCount())
  {
    while (proximity->getStatus() != C_PCKT_RX)
    {
      BLE.poll();
    }

    switch (proximity->getStatus())
    {
    case C_PCKT_RX:
      proximity->incrementTXPcktIndex();
      sendChunkAwaitAck();
      break;
    case C_PCKT_LOSS:
      sendChunkAwaitAck();
      break;
    default:
      break;
    }
  }
}

///@brief Send chunk and awaits acknowledgment
void sendChunkAwaitAck()
{
  byte *chunkToTransmit = proximity->chunkToTransfer();

  packetTransferChar.setValue(chunkToTransmit, PACKET_SIZE + 1);
  transferStatusChar.writeValue(P_PCKT_TX);
  transferStatus = P_PCKT_TX;
}