#include <Arduino.h>
#include "SSD1306Wire.h"
#include <SPI.h>
#include <LoRa.h>

#include "../header/WeatherInfo.h"
// #include "../header/WeatherCard.h"
#include "../header/SIM800_ESP32.h"
#include "../header/aicimages.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

#define RXD2 16
#define TXD2 17

SSD1306Wire display(0x3C, SDA, SCL); 

SIM800_ESP32 SMS_Sender;
WeatherInfo KuyaKim;
// WeatherCard KimCard;
  
void setup() {  
  
  Serial.begin(115200);
  // Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  while (!Serial);

  Serial.println("LoRa Receiver");

  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.drawXbm(0, 0, 128, 64, epd_bitmap_AIC_WSRECEIVER);
  display.invertDisplay();
  delay(2500);
  display.display();
  delay(2500);
  display.invertDisplay();

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(868E6)) {
    while (1);
  }

  // register the receive callback
  // LoRa.onReceive(Received);

  // put the radio into receive mode
  // LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // read packet
    KuyaKim.clearString();
    while (LoRa.available()) 
    {
      KuyaKim.JSONString += (char)LoRa.read();
    }
  
    if(KuyaKim.deSerialize() == EXIT_SUCCESS)
    {
      KuyaKim.updateValues();

      // KimCard.updateCards(KuyaKim);
    }
  } else
  {
    Serial.println("No packets received");
  }

  delay(500);
}

/*
void Received(int packetSize)
{
  KuyaKim.clearString();
  // while (LoRa.available())
  for (int i = 0; i < packetSize; i++) 
  {
    KuyaKim.JSONString += (char)LoRa.read();
  }

  if(KuyaKim.deSerialize() == EXIT_SUCCESS)
  {
    KuyaKim.updateValues();

    // KimCard.updateCards(KuyaKim);
  }
}
*/