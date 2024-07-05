/**********************************************************
** @file		AICWeather.cpp
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

#include <Arduino.h>
#include <cmath>
#include <DHT22.h>
#include <BMP388_DEV.h>
#include "AICWeather.h"

#define DEBOUNCE_TIME 15
#define CALC_INTERVAL 1000
#define M_180_PI (180 / M_PI)

volatile int _anemometerCounter;
volatile int _rainCounter;
volatile unsigned long last_micros_rg;
volatile unsigned long last_micros_an;

double rawvalWindDirPin1;
double rawvalWindDirPin2;

double valWindDirPin1;
double valWindDirPin2;

double mappedWindDirPin1;
double mappedWindDirPin2;
double windDirection;

//Initialization routine. This functrion sets up the pins on the Arduino and initializes variables.

AICWeather::AICWeather()
{
	dht22 = new DHT22(15);
	bmp388 = new BMP388_DEV(21, 22);
}

AICWeather::AICWeather(int rainPin, int windDirPin1, int windDirPin2, 
												int windSpdPin, int dhtPin, int bmp388Pin_SDA = 21, int bmp388Pin_SCL = 22)
{
  //Initialization routine
  _anemometerCounter = 0;
  _rainCounter = 0;
  _gustIdx = 0;
  _vaneSampleIdx = 0;
  
  _rainPin = rainPin;
  _windDirPin1 = windDirPin1;
  _windDirPin2 = windDirPin2;
  _windSpdPin = windSpdPin;
  _dhtPin = dhtPin;

  dht22 = new DHT22(_dhtPin);  
  bmp388 = new BMP388_DEV(bmp388Pin_SDA, bmp388Pin_SCL);

  pinMode(_rainPin, INPUT_PULLUP);
  pinMode(_windSpdPin, INPUT_PULLUP);

  bmp388->begin();
  bmp388->setTimeStandby(TIME_STANDBY_1280MS);
  bmp388->startNormalConversion();
}

//The update function updates the values of all of the sensor variables. This should be run as frequently as possible
//in the main loop for maximum precision.
void AICWeather::update()
{
	int result;
	_timer = millis();

	_vaneSample[_vaneSampleIdx] = _readWindDir();
	_vaneSampleIdx++;

	if(_vaneSampleIdx >= windDirMaxIter)
	{
		_vaneSampleIdx = 0;
	}
	if(_timer > _nextCalc)
	{
		_nextCalc = _timer + CALC_INTERVAL;
 
		//UPDATE ALL VALUES
		// bmp388->getMeasurements(_temp, _pressure, _altitude)
		_rain += _readRainAmmount();
		_windSpd = _readWindSpd();
		_windDir = _getWindDir();
		_temp = dht22->getTemperature();
		_humidity = dht22->getHumidity();
		_heatIndex = _calcHeatIndex();
	}
}

//Returns the ammount of rain since the last time the getRain function was called.
float AICWeather::getRain()
{
	return _rain/1000.0;
}

//Returns the direction of the wind in degrees.
float AICWeather::getWindDirection()
{
	return _windDir;
}

//Returns the wind speed.
float AICWeather::getWindSpeed()
{
	float temp = _windSpd/10.0; 
	return temp;
}

//Returns the maximum wind gust speed. 
float AICWeather::getWindGust()
{
	float temp = _windSpdMax / 10.0; 
	return temp % 10.0;
}

float AICWeather::getTemp()
{
	return _temp;
}

float AICWeather::getHumidity()
{
	return _humidity;
}

float AICWeather::getHeatIndex()
{
	return _heatIndex;
}

float AICWeather::getPressure()
{
	return _pressure;
}

float AICWeather::getAltitude()
{
	return _altitude;
}

float AICWeather::_calcHeatIndex()
{
	float T = (_temp * 1.8) + 32;

	float _tempHI = c1 + c2*T + c3*(_humidity) + c4*T*(_humidity) + c5*(T^2) + c6*(_humidity^2) + c7*(T^2)*(_humidity) + c8*T*(_humidity^2) + c9*(T^2)*(_humidity^2);

	return _tempHI;
}

//Updates the rain ammmount internal state.
float AICWeather::_readRainAmmount()
{
	int rain = 0;
	rain = 11 * _rainCounter;
	_rainCounter = 0;
	return rain;
} 

//Updates the wind direction internal state.
float AICWeather::_readWindDir()
{
	rawvalWindDirPin1 = analogRead(_windDirPin1);
	rawvalWindDirPin2 = analogRead(_windDirPin2);

	mappedWindDirPin1 =  ((rawvalWindDirPin1 / 4095.0) * 5) - 2.5;
	mappedWindDirPin2 =  ((rawvalWindDirPin2 / 4095.0) * 5) - 2.5;

	mappedWindDirPin1 = mappedWindDirPin1 / 2.5 ;
	mappedWindDirPin2 = mappedWindDirPin2 / 2.5 ;

	windDirection = atan2(mappedWindDirPin2, mappedWindDirPin1);

	windDirection = windDirection * M_180_PI;

	windDirection = (signbit(windDirection) == 1) ? windDirection + 360 : windDirection;

	windDirection += 45;

	windDirection = (windDirection < 90) ? windDirection + 270 : windDirection - 90;

	return static_cast<float>(windDirection);
 }

float AICWeather::_getWindDir()
{
	float sum = 0;
	float ave;
	for (int i = 0; i < windDirMaxIter; ++i)
	{
		sum += _vaneSample[i];
	}
	ave = sum / windDirMaxIter;

	return ave;
}

//returns the wind speed since the last calcInterval.
float AICWeather::_readWindSpd()
{
	unsigned char i;
	
	long spd = 14920;
	spd *= _anemometerCounter;
	spd /= 1000;
	_anemometerCounter = 0;
	_gustIdx++;

	if(_gustIdx > 29)
	{
		_gustIdx = 0;
	}

	_gust[_gustIdx] = (int) spd;

	for (i = 0; i < 30; i++)
	{
		if (_gust[i] > _windSpdMax) _windSpdMax = _gust[i];	
	}

	return (float) spd;
}

//ISR for rain gauge.
void AICWeather::countRain()
{

	if((long)(micros() - last_micros_rg) >= DEBOUNCE_TIME * 1000)
	{
		
		_rainCounter++;
		last_micros_rg = micros();
	}
	
}

//ISR for anemometer.
void AICWeather::countAnemometer()
{
	if((long)(micros() - last_micros_an) >= DEBOUNCE_TIME * 1000)
	{
		_anemometerCounter++;
		last_micros_an = micros();
	}
}
