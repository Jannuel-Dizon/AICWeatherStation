#include "../header/AICWeather.h"
#include "../header/AICWeather.cpp"
#include "../header/WeatherInfo.h"
#include "../header/aicimages.h"

//Libraries Included
#include <Wire.h>
#include "SSD1306Wire.h"
#include <SPI.h>
#include <LoRa.h>

//Sensor Pins
#define rainPin 4
#define wdPin1 13
#define wdPin2 12
#define wsPin 2
#define dhtPin 15
// #define relayPin 

//LoRa Pins
#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

int Transmitter_CALC_INTERVAL = 1000;

int counter;

unsigned long nextCalc;
unsigned long timer;

SSD1306Wire display(0x3C, SDA, SCL); 

// AICWeather::AICWeather(int rainPin, int windDirPin1, int windDirPin2, int windSpdPin, int dht11Pin)
AICWeather ws1(rainPin, wdPin1, wdPin2, wsPin, dhtPin);
WeatherInfo KuyaKim;

void setup(){
  Serial.begin(115200);
  while (!Serial);

  display.init();
  display.setContrast(255);
  display.drawXbm(0, 0, 128, 64, epd_bitmap_AIC_WSTRANSMITTER);
  display.invertDisplay();
  delay(2500);
  display.display();
  delay(2500);
  display.invertDisplay();

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  attachInterrupt(digitalPinToInterrupt(rainPin), ws1.countRain, FALLING); //ws1.countRain is the ISR for the rain gauge.
  attachInterrupt(digitalPinToInterrupt(wsPin), ws1.countAnemometer, FALLING); //ws1.countAnemometer is the ISR for the anemometer.
  nextCalc = millis() + Transmitter_CALC_INTERVAL;
}

void loop(){
  // if (Serial.available() > 0) {
  // Transmitter_CALC_INTERVAL = Serial.read();
  // }
  timer = millis();

  ws1.update(); //Call this every cycle in your main loop to update all the sensor values

  if(timer > nextCalc)
  {
    nextCalc = timer + Transmitter_CALC_INTERVAL;

    KuyaKim.clearString();

    KuyaKim.updateValues(&ws1);
    
    KuyaKim.updateDocs();
    KuyaKim.serialize();
    
    // Send JSON data over Serial
    Serial.println(KuyaKim.JSONString);
    LoRa.beginPacket();
    LoRa.print(KuyaKim.JSONString);
    LoRa.endPacket();
  }

  delay(5000);

}