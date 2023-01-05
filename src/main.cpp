#include <Arduino.h>
#include <BluetoothSerial.h>
#include <M5StickCPlus.h>
#undef min
#include <EELSP.h>

#define drawPixel(a, b, c)            \
    M5.Lcd.setAddrWindow(a, b, a, b); \
    M5.Lcd.pushColor(c)

bool isConnected = false;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(1);
}

void loop() {
  M5.update();
  
  // reset function
  if(M5.BtnB.isPressed()) {
    EELSP.reset();
  }

  if(!isConnected && WiFi.status() == WL_CONNECTED) {
    isConnected = true;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
  }

  if(M5.BtnA.isPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
  }
}