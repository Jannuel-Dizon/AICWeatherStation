#include <Arduino.h>

#include <SPI.h>
#include <LoRa.h>

#include "../header/WeatherInfo.h"
// #include "../header/WeatherCard.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

WeatherInfo KuyaKim;
// WeatherCard KimCard;

void setup() {
  Serial.begin(115200);

  while (!Serial);

  Serial.println("LoRa Receiver");

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

    Serial.println("");
  
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