#ifndef WEATHERINFO_H
#define WEATHERINFO_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "AICWeather.h"

// Sample JSONString
// "{\"wind_speed\":\"10\",\"wind_gust\":\"20\",\"wind_direction\":\"180\",\"total_rain\":\"5\",\"temperature\":\"35\",\"humidity\":\"75\",\"heat_index\":\"38\",\"atm_pressure\":\"1.009\"}"

/*
{
  "wind_speed" : "10",
  "wind_gust" : "20",
  "wind_direction" : "180",
  "total_rain" : "5",
  "temperature" : "35",
  "humidity" : "75",
  "heat_index" : "38", 
  "atm_pressure":"1.009"
}
*/

struct WeatherInfo 
{
  StaticJsonDocument<512> doc;
  // JsonDocument doc;
  String JSONString;
  float windSpeed;
  float windGust;
  float windDirection;
  float totalRain;
  float temperature;
  float humidity;
  float heatIndex;
  float atmPressure;

  WeatherInfo();
  int deSerialize();
  void serialize();
  void clearString();
  void updateValues();
  void updateValues(AICWeather *ws1);
  void printValues();
  void updateDocs();
  float getWindSpeed();
  float getWindGust();
  float getWindDirection();
  float getTotalRain();
  float getTemperature();
  float getHumidity();
  float getHeatIndex();
  float getAtmPressure();
};

WeatherInfo::WeatherInfo() : windSpeed(0.00), windGust(0.00), windDirection(0.00),
                              totalRain(0.00), temperature(0.00), humidity(0.00),
                              heatIndex(0.00), atmPressure(0.00), 
                              JSONString("{\"wind_speed\":\"0.00\",\"wind_gust\":\"0.00\",\"wind_direction\":\"0.00\",\"total_rain\":\"0.00\",\"temperature\":\"0.00\",\"humidity\":\"0.00\",\"heat_index\":\"0.00\",\"atm_pressure\":\"0.00\"}")
{

}

int WeatherInfo::deSerialize()
{
  DeserializationError error = deserializeJson(doc, JSONString);
  if(error)
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void WeatherInfo::serialize()
{
  serializeJson(doc, JSONString);
}

void WeatherInfo::clearString()
{
  JSONString = "";
}

void WeatherInfo::updateValues()
{
  windSpeed = doc["wind_speed"];
  windGust = doc["wind_gust"];
  windDirection = doc["wind_direction"];
  totalRain = doc["total_rain"];
  temperature = doc["temperature"];
  humidity = doc["humidity"];
  heatIndex = doc["heat_index"];
  atmPressure = doc["atm_pressure"];
}

void WeatherInfo::updateValues(AICWeather *ws1)
{
  windSpeed = ws1->getWindSpeed();
  windGust = ws1->getWindGust();
  windDirection = ws1->getWindDirection();
  totalRain = ws1->getRain();
  temperature = ws1->getTemp();
  humidity = ws1->getHumidity();
  heatIndex = ws1->getHeatIndex();
  atmPressure = ws1->getAtmPressure();
}

void WeatherInfo::updateDocs()
{
  doc["wind_speed"] = windSpeed;
  doc["wind_gust"] = windGust;
  doc["wind_direction"] = windDirection;
  doc["total_rain"] = totalRain;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["heat_index"] = heatIndex;
  doc["atm_pressure"] = atmPressure;
}

void WeatherInfo::printValues()
{
  Serial.println();
  Serial.print("Wind Speed: ");
  Serial.println(windSpeed);
  Serial.print("Wind Gust: ");
  Serial.println(windGust);
  Serial.print("Wind Direction: ");
  Serial.println(windDirection);
  Serial.print("Total Rain: ");
  Serial.println(totalRain);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Heat Index: ");
  Serial.println(heatIndex);
  Serial.print("Atmospheric Pressure: ");
  Serial.println(atmPressure);
}

float WeatherInfo::getWindSpeed()
{
  return windSpeed;
}

float WeatherInfo::getWindGust()
{
  return windGust;
}

float WeatherInfo::getWindDirection()
{
  return windDirection;
}

float WeatherInfo::getTotalRain()
{
  return totalRain;
}

float WeatherInfo::getTemperature()
{
  return temperature;
}

float WeatherInfo::getHumidity()
{
  return humidity;
}

float WeatherInfo::getHeatIndex()
{
  return heatIndex;
}

float WeatherInfo::getAtmPressure()
{
  return atmPressure;
}


#endif