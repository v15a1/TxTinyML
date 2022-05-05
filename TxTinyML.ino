#include <ArduinoBLE.h>
#include "Proximity.h"

#define PACKET_SIZE 128 // Maximum payload

BLEService transferService("03CCB8A4-0C70-4D63-8EBE-E8F6FA339FB7");

// Characteristics used for transferring
BLEUnsignedCharCharacteristic transferStatusChar("2cfca732-7f87-4e54-a00f-a7be43d6bf49", BLERead | BLEWrite | BLEWriteWithoutResponse); // remote clients will be able to get notifications if this characteristic changes
BLEIntCharacteristic transferPacketCountChar("9d4a4fc1-8eeb-4785-859b-32c584a46f5e", BLERead | BLENotify);                              // remote clients will be able to get notifications if this characteristic changes
BLECharacteristic packetTransferChar("5b612497-2af9-46c8-b532-48bd186035f3", BLERead | BLENotify, (PACKET_SIZE + 1));                   // remote clients will be able to get notifications if this characteristic changes
// BLEBooleanCharacteristic decisionNotifier("1f1c1b93-87a0-4d85-b63a-776c4e8a1acf", BLERead | BLENotify);                                 // remote clients will be able to get notifications if this characteristic changes

Proximity *proximity = new Proximity();

void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}
