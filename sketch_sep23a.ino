#include <ArduinoBLE.h>
#include <string>
#include <vector>

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// UUIDs --------------------------------------------------------------------------------------------------------------
const char * deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char * deviceServiceReadColorUuid = "19b10001-e8f2-537e-4f6c-d104768a1215";
const char * deviceServiceWriteColorUuid = "19b10001-e8f2-537e-4f6c-d104768a1216";
const char * deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1217";

// Service + Charactaristics ------------------------------------------------------------------------------------------
BLEService servoService(deviceServiceUuid);
BLEStringCharacteristic deviceServiceReadColor(deviceServiceReadColorUuid, BLERead, 6);
BLEStringCharacteristic deviceServiceWriteColor(deviceServiceWriteColorUuid, BLEWrite, 6);
BLEStringCharacteristic deviceServiceNotifyColor(deviceServiceCharacteristicUuid, BLENotify, 6);

// State Variables -----------------------------------------------------------------------------------------------------
int counter = 0;
String allColors[] = {"R", "G", "B"};
int colorArrayLength = 3;
bool isNotifying = true;

// Utilities for Drawing ------------------------------------------------------------------------------------------------
void drawMessage(const char* message) {
  matrix.clear();
  matrix.textFont(Font_4x6);
  matrix.beginText(1, 1, 0xFFFFFF);
  matrix.println(message);
  matrix.endText();
  matrix.end();
}

void initMatrix() {
  matrix.begin();
  drawMessage("SCN");
}

// Initialize BLE --------------------------------------------------------------------------------------------------------
void initBLE() {
  BLE.setDeviceName("Arduino");
  BLE.setLocalName("Arduino");

  if(!BLE.begin()) {
    Serial.println("- Starting Bluetooth Low Energy Module Failed");
    while(1);
  }

  BLE.setAdvertisedService(servoService);
  servoService.addCharacteristic(deviceServiceReadColor);
  servoService.addCharacteristic(deviceServiceWriteColor);
  servoService.addCharacteristic(deviceServiceNotifyColor);
  BLE.addService(servoService);

  deviceServiceReadColor.writeValue("O");
  deviceServiceNotifyColor.setValue("R");
  BLE.advertise();
}

void setup() {
  Serial.begin(115200);
  initMatrix();
  initBLE();
}

// Main Loop --------------------------------------------------------------------------------------------------------------
void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- DISCOVERING CENTRAL DEVICE");
  
  if(central) {
    Serial.println("CONNECTED ");
    drawMessage("RDY");

    while (central.connected()) {
      if(deviceServiceWriteColor.written()) {
        String value = deviceServiceWriteColor.value();
        Serial.println(value);

        if(value == "START") {
          isNotifying = true;
        }
      }
      if(isNotifying) {
        int index = counter % colorArrayLength;
        
        switch(index) {
          case 0: 
            drawMessage(" R");
            break;
          case 1:
            drawMessage(" G");
            break;
          case 2:
            drawMessage(" B");
            break;
        } 
        deviceServiceNotifyColor.setValue(allColors[index]);
        counter++;
        delay(500);
      } 
    }
  }
}
