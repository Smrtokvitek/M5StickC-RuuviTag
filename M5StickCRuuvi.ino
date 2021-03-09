/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>

#include <M5StickC.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      
      if (advertisedDevice.getAddress().toString() == "e5:ca:f3:f1:7d:ea") {
        
        //digitalWrite(M5_LED, LOW);
        
        char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
        std::stringstream ss;
        ss << pHex;
        free(pHex);
        
        Serial.println(ss.str().c_str());
  
        std::string hum = ss.str().substr(6, 2);
        float humidity = strtol(hum.c_str(), 0, 16);
        humidity = humidity / 2;
        
        std::string temp1 = ss.str().substr(8, 2);
        std::string temp2 = ss.str().substr(10, 2);
        float temp = strtol(temp1.c_str(), 0, 16) + (float)strtol(temp2.c_str(), 0, 16) / 100;
        if (temp > 128) {
          temp -= 128;
          temp = 0 - temp;
        }

        std::string press1 = ss.str().substr(12, 2);
        std::string press2 = ss.str().substr(14, 2);
        uint32_t pressure = strtoul(press2.c_str(), 0, 16) | strtoul(press1.c_str(), 0, 16) << 8;
        pressure = pressure + 50000;

        std::string batt1 = ss.str().substr(20, 2);
        std::string batt2 = ss.str().substr(22, 2);
        uint32_t batt = strtoul(batt2.c_str(), 0, 16) | strtoul(batt1.c_str(), 0, 16) << 8;

        //M5.Lcd.fillScreen(BLACK);
        M5.Lcd.fillScreen(DARKGREY);
        M5.Lcd.fillCircle(145, 10, 6, BLUE);
        M5.Lcd.setCursor(0,0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println(advertisedDevice.getAddress().toString().c_str());
        M5.Lcd.println();
        M5.Lcd.setTextSize(2);
        M5.Lcd.print("T: ");
        M5.Lcd.print(temp);
        M5.Lcd.println(" C");
        M5.Lcd.print("H: ");
        M5.Lcd.print(humidity);
        M5.Lcd.println(" %");
        M5.Lcd.print("P: ");
        M5.Lcd.print((float)pressure/100);
        M5.Lcd.println(" hPa");
        //M5.Lcd.print((float)batt);

        delay(100);
        //M5.Lcd.fillCircle(145, 10, 6, BLACK);
        M5.Lcd.fillCircle(145, 10, 6, DARKGREY);
        //digitalWrite(M5_LED, HIGH);
      }
    }
};

void setup() {
  //Serial.begin(115200);

  M5.begin();
  M5.Axp.ScreenBreath(9);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);

  //pinMode(M5_LED, OUTPUT);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(400);
}
