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

#ifndef AICWEATHER_H
#define AICWEATHER_H

#include "Arduino.h"
#include <DHT22.h>

#define windDirMaxIter 10

// constants for Heat Index
#define c1 -42.379
#define c2 2.04901523
#define c3 10.14333127
#define c4 -0.22475541
#define c5 -6.83783E-3
#define c6 -5.481717E-2
#define c7 1.22874E-3
#define c8 8.5282E-4
#define c9 -1.99E-6

// constants for ATM Pressure
#define AIC_p0 101325
#define AIC_h0 0
#define AIC_g 9.80665 
#define AIC_M 0.0289644
#define AIC_R 8.31432

class AICWeather
{
  public:
	AICWeather();
  AICWeather(int, int, int, int, int);
    
	float getWindSpeed();
	float getWindGust();
	float getWindDirection();
	float getRain();
	float getTemp();
	float getHumidity();
	float getHeatIndex();
	float getAltitude();
	float getAtmPressure();
	
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

	float _windSpd;
	float _windSpdMax;
	float _windDir;
	float _rain;
	float _temp;
	float _humidity;
	float _heatIndex;
	float _altitude;
	float _atmPressure;
	
	unsigned long _nextCalc;
	unsigned long _timer;
  	
	int _vaneSample[windDirMaxIter]; //50 samples from the sensor for consensus averaging
	unsigned int _vaneSampleIdx;
	
	unsigned int _gust[30]; //Array of 50 wind speed values to calculate maximum gust speed.
	unsigned int _gustIdx;
	
	float _calcAtmPressure();
	float _calcHeatIndex();
	float _readRainAmmount();
  float _readWindDir();
  float _getWindDir();
  float _readWindSpd();
	
};

//static void countRain();

#endif
