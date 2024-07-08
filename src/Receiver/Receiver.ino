#include <Arduino.h>
#include "SSD1306Wire.h"
#include <SPI.h>
#include <LoRa.h>
#include <mySD.h>

#include "../header/WeatherInfo.h"
// #include "../header/WeatherCard.h"
#include "../header/SIM800_ESP32.h"
#include "../header/aicimages.h"

ext::File root;

#define  oRST        16 

#define  SD_CLK     17
#define  SD_MISO    13
#define  SD_MOSI    12
#define  SD_CS      23

#define  LoRa_SCK    5
#define  LoRa_MISO  19
#define  LoRa_MOSI  27
#define  LoRa_CS    18
#define  LoRa_RST   14    //  LoRa_Reset
#define  DI0        26
#define  BAND    868E6 
#define  Select    LOW   //  Low CS means that SPI device Selected
#define  DeSelect  HIGH  //  High CS means that SPI device Deselected

#define RXD2 3
#define TXD2 1

SSD1306Wire display(0x3C, SDA, SCL); 

SIM800_ESP32 SMS_Sender;
WeatherInfo KuyaKim;
ext::File sessionFile;   //  SD card filenames are restricted to 8 characters + extension
// WeatherCard KimCard;

String tempy = "";

void setup() {  
  // set output pins
  pinMode(oRST,OUTPUT);
  pinMode(SD_CS,OUTPUT);
  pinMode(LoRa_CS,OUTPUT);
  digitalWrite(LoRa_CS, DeSelect);

  // set GPIO16 Low then High to Reset OLED
  digitalWrite(oRST, LOW);  
  delay(50);
  digitalWrite(oRST, HIGH);
  
  Serial.begin(115200);

  while (!Serial);

  Serial.print("Initializing SD card...");
  digitalWrite(SD_CS, Select);    //  SELECT (Low) SD Card SPI
/**/
  if (!SD.begin( SD_CS, SD_MOSI, SD_MISO, SD_CLK )) {
    Serial.println("initialization failed!");
    //  now what?
  } else {
    Serial.println("initialization done.");
    delay(1000);
  }
  /* open "test.txt" for writing */
  root = SD.open("datas.txt", FILE_WRITE);
  if (root) {
    root.println("TESTING");
    root.flush();
    root.close();
  } else {    //  file open error
    Serial.println("error opening datas.txt");
  }
  delay(100);

  /* after writing, then reopen the file and read it */
  root = SD.open("datas.txt");
  if (root) {    /* read from the file to the end of it */
    while (root.available()) {  // read the file
      Serial.write(root.read());
    }
    root.close();
  } else {
    Serial.println("error opening datas.txt");
  }
  delay(100);

  //  done testing the SD Card
  digitalWrite(SD_CS, DeSelect); 
//  SD.end();
  delay( 100 ); 

  SPI.begin( LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_CS );
  LoRa.setPins( LoRa_CS, LoRa_RST, DI0 );
  digitalWrite(LoRa_CS, Select);   //  SELECT (low) LoRa SPI 
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");

  } else {
    Serial.println("LoRa Initial OK!");
    delay(1000);
  }
  digitalWrite(LoRa_CS, DeSelect);  
  Serial.println("Setup done!");

  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.drawXbm(0, 0, 128, 64, epd_bitmap_AIC_WSRECEIVER);
  display.invertDisplay();
  delay(2500);
  display.display();
  delay(2500);
  display.invertDisplay();


}

void loop() {
  digitalWrite(LoRa_CS, Select);
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
      KuyaKim.printValues();
      // KimCard.updateCards(KuyaKim);
    }
    if(tempy != KuyaKim.JSONString)
    {
      tempy = KuyaKim.JSONString;  
      digitalWrite(LoRa_CS, DeSelect);
      delay(50);         
      digitalWrite(SD_CS, Select);    
      root = SD.open("datas.txt", FILE_WRITE);
      Serial.println(root);
    }
    if (root) 
    {
      root.println(tempy);
      root.flush();
      root.close();   
    }
    Serial.println(tempy); 
    digitalWrite(SD_CS, DeSelect);  
    delay(500);
  } 
  else
  {
    Serial.print(".");
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