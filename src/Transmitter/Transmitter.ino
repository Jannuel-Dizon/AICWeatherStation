#include "header/AICWeather.h"

//Libraries Included
#include <Wire.h>
#include "SSD1306Wire.h"
#include <AICWeather.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

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

int CALC_INTERVAL = 10000;

SSD1306Wire display(0x3c, SDA, SCL);

int windDirshon;
int rainAmount;
int windSpeed;
int temperature;
int humidity;
int hic;
int counter;

unsigned long nextCalc;
unsigned long timer;
// AICWeather::AICWeather(int rainPin, int windDirPin1, int windDirPin2, int windSpdPin, int dht11Pin)
AICWeather ws1(rainPin, wdPin1, wdPin2, wsPin, dhtPin);

// AICWeather ws1;

void setup(){
  Serial.begin(115200);
  while (!Serial);

  display.init();
  // display.flipScreenVertically();
  display.setContrast(255);

  display.setFont(ArialMT_Plain_24);
  display.println("Transmitter");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }



  attachInterrupt(digitalPinToInterrupt(rainPin), ws1.countRain, FALLING); //ws1.countRain is the ISR for the rain gauge.
  attachInterrupt(digitalPinToInterrupt(wsPin), ws1.countAnemometer, FALLING); //ws1.countAnemometer is the ISR for the anemometer.
  nextCalc = millis() + CALC_INTERVAL;
}

void loop(){
  // if (Serial.available() > 0) {
  // CALC_INTERVAL = Serial.read();
  // }
  timer = millis();

  int rainAmount;
  long windSpeed;
  long windDirshon;
  int windGust;

  ws1.update(); //Call this every cycle in your main loop to update all the sensor values

  if(timer > nextCalc)
  {
    nextCalc = timer + CALC_INTERVAL;
    rainAmount = ws1.getRain();
    windSpeed = ws1.getWindSpeed();
    windDirshon = ws1.getWindDirection();
    windGust = ws1.getWindGust();
    temperature = ws1.getTemp();
    humidity = ws1.getHumidity();
    hic = ws1.getHeatIndex();

//     windSpeed / 10 will give the integer component of the wind speed
//     windSpeed % 10 will give the fractional component of the wind speed
    // Serial.print("Wind speed: ");
    // Serial.print(windSpeed / 10);
    // Serial.print('.');
    // Serial.print(windSpeed % 10);
    // Serial.print(" ");

    // Serial.print("Gusting at: ");
    // Serial.print(windGust / 10);
    // Serial.print('.');
    // Serial.print(windGust % 10);
    // Serial.println("");

    StaticJsonDocument<512> doc;

    //Add weather data to the JSON document
    doc["wind_speed"] = windSpeed;
    doc["wind_gust"] = windGust;
    doc["wind_direction"] = windDirshon;
    doc["total_rain"] = rainAmount;

    // Add DHT22 sensor data to the JSON document
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["heat_index"] = hic;


    // Serialize JSON to a char buffer
    char jsonDerulo[512];
    serializeJson(doc, jsonDerulo);
    
    // Send JSON data over Serial
    Serial.println(jsonDerulo);
    LoRa.beginPacket();
    LoRa.print(jsonDerulo);
    LoRa.endPacket();
    // display.setFont(ArialMT_Plain_24);
    // display.println("Transmitter");

  }

}