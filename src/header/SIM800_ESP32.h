#ifndef SIM800_ESP32_H
#define SIM800_ESP32_H

#include <Arduino.h>

#define RXD2 16
#define TXD2 17

class SIM800_ESP32
{
public:
	SIM800_ESP32();
	void sendSMS(String PhoneNumber, String Message);
};


SIM800_ESP32::SIM800_ESP32()
{
	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void SIM800_ESP32::sendSMS(String PhoneNumber, String Message)
{
	Serial2.println("AT+CMGF=1"); // Configuring TEXT mode
  Serial2.println("AT+CMGS="+PhoneNumber); // +ZZxxxxxxxxxx format +639064294458
  Serial2.print(Message); //text content
  Serial2.write(26);
}


#endif