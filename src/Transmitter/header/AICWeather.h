/**********************************************************
** @file		ADSWeather.cpp
** @author		John Cape
** @copyright	Argent Data Systems, Inc. - All rights reserved
**
** Argent Data Systems weather station Arduino library.
** This library provides a set of functions for interfacing
** with the Argent Data Systesm weather station sensor package
** These sensors consist of a rain gauge, a wind vane and an
** anemometer. The anemometer and the rain gauge should be 
** connected to digital input pins on one side and ground on
** the other. The weather vane is a variable resistor. 
** It should be connected to an analog input pin on one side
** and ground on the other. The analog input pin needs to be 
** connected to 5V from the Arduion through a 10K Ohm resistor.
**

*/

#ifndef AICWeather_h
#define AICWeather_h

#include "Arduino.h"
#include <DHT22.h>
// #include <BMP388_DEV.h>

#define windDirMaxIter 10

// For getting Heat Index
#define c1 -42.379
#define c2 2.04901523
#define c3 10.14333127
#define c4 -0.22475541
#define c5 -6.83783E-3
#define c6 -5.481717E-2
#define c7 1.22874E-3
#define c8 8.5282E-4
#define c9 -1.99E-6


class AICWeather
{
  public:
	AICWeather();
  AICWeather(int rainPin, int windDirPin1, int windDirPin2, 
  						int windSpdPin, int dhtPin, int bmp388Pin_SDA = 21, int bmp388Pin_SCL = 22);
    
	float getRain();
	float getWindDirection();
	float getWindSpeed();
	float getWindGust();
	float getTemp();
	float getHumidity();
	float getHeatIndex();
	float getPressure();
	float getAltitude();
	
	void update();
	
	static void countRain();
	static void countAnemometer();

	
  private:
	int _rainPin;
	int _windDirPin1;
	int _windDirPin2;
	int _windSpdPin;
	int _dhtPin;

	DHT22 *dht22;

	// BMP388_DEV *bmp388;
	
	float _rain;
	float _windDir;
	float _windSpd;
	float _windSpdMax;
	float _temp;
	float _humidity;
	float _heatIndex;
	float _pressure;
	float _altitude;
	
	unsigned long _nextCalc;
	unsigned long _timer;
  	
	int _vaneSample[windDirMaxIter]; //50 samples from the sensor for consensus averaging
	unsigned int _vaneSampleIdx;
	
	unsigned int _gust[30]; //Array of 50 wind speed values to calculate maximum gust speed.
	unsigned int _gustIdx;
	
	float _calcHeatIndex();
	float _readRainAmmount();
  float _readWindDir();
  float _getWindDir();
  float _readWindSpd();
	
};

//static void countRain();

#endif