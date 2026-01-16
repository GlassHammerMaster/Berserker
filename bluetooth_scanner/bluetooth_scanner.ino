#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buttons
int buttonUp     = 15;
int buttonDown   = 18;
int buttonSelect = 12;

// UI layout
int menuStartY = 10;
int lineHeight = 10;
int visibleLines = (SCREEN_HEIGHT - menuStartY) / lineHeight;

// BLE
#define MAX_DEVICES 25
#define SCAN_TIME 5

BLEScan* pBLEScan;

String devices[MAX_DEVICES];
int deviceCount = 0;

int selected = 0;
int scrollOffset = 0;

// ---------------- BLE CALLBACK ----------------
class ScanCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice d) {
    if (deviceCount >= MAX_DEVICES) return;

    String entry;
    if (d.haveName())
      entry = d.getName().c_str();
    else
      entry = d.getAddress().toString().c_str();

    // Prevent duplicates
    for (int i = 0; i < deviceCount; i++) {
      if (devices[i] == entry) return;
    }

    devices[deviceCount++] = entry;
  }
};

// ---------------- BLE SCAN ----------------
void scanBLE() {
  deviceCount = 0;
  selected = 0;
  scrollOffset = 0;

  display.clearDisplay();
  display.setCursor(0, 30);
  display.setTextColor(SSD1306_WHITE);
  display.println("Scanning BLE...");
  display.display();

  pBLEScan->start(SCAN_TIME, false);
  pBLEScan->clearResults();

  drawList();
}

// ---------------- DRAW LIST ----------------
void drawList() {
  display.clearDisplay();

  for (int i = 0; i < visibleLines; i++) {
    int index = scrollOffset + i;
    if (index >= deviceCount) break;

    int y = menuStartY + i * lineHeight;

    if (index == selected)
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else
      display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, y);
    display.println(devices[index]);
  }

  display.display();
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  display.clearDisplay();
  display.setCursor(0, 30);
  display.setTextColor(SSD1306_WHITE);
  display.println("BLE Scanner");
  display.display();
  delay(1500);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new ScanCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(80);

  scanBLE();
}

// ---------------- LOOP ----------------
void loop() {

  // UP
  if (digitalRead(buttonUp) == LOW && deviceCount > 0) {
    selected--;
    if (selected < 0) selected = deviceCount - 1;
    if (selected < scrollOffset) scrollOffset = selected;
    drawList();
    delay(150);
  }

  // DOWN
  if (digitalRead(buttonDown) == LOW && deviceCount > 0) {
    selected++;
    if (selected >= deviceCount) selected = 0;
    if (selected >= scrollOffset + visibleLines)
      scrollOffset = selected - visibleLines + 1;
    drawList();
    delay(150);
  }

  // SELECT
  if (digitalRead(buttonSelect) == LOW && deviceCount > 0) {
    Serial.print("Selected device: ");
    Serial.println(devices[selected]);

    display.clearDisplay();
    display.setCursor(0, 30);
    display.setTextColor(SSD1306_WHITE);
    display.println("Selected:");
    display.println(devices[selected]);
    display.display();

    delay(1200);
    drawList();
  }
}
