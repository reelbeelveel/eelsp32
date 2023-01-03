#include <Arduino.h>
#include <M5StickCPlus.h>
#include <BluetoothSerial.h>
#include <EELSP.h>

#define SSID       "Nighthawk"
#define PASSWD     "Attheendoftheday"
#define STATIC_IP  IPAddress(192, 168, 1, 32)
#define GATEWAY_IP IPAddress(192, 168, 1, 1)
#define SUBNET_IP  IPAddress(255, 255, 255, 0)

#define drawPixel(a, b, c)            \
    M5.Lcd.setAddrWindow(a, b, a, b); \
    M5.Lcd.pushColor(c)

bool isConnected = false;

void initializeWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.config(STATIC_IP, GATEWAY_IP, SUBNET_IP);
    WiFi.begin(SSID, PASSWD);
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(1);
  initializeWiFi();
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
    M5.Lcd.printf("Connected to %s", SSID);
  }

  if(M5.BtnA.isPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
  }
}