#ifndef WEATHERCARD_H
#define WEATHERCARD_H

#include <Arduino.h>
#if defined(ESP8266)
  /* ESP8266 Dependencies */
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#elif defined(ESP32)
  /* ESP32 Dependencies */
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <ESPDash.h>

#include "WeatherInfo.h"

#define SSID "Weather_Station"
#define password "aicisthebest"

struct WeatherCard
{
	const String ssid = SSID; // SSID
	const String password = password; // Password
	AsyncWebServer server;
	ESPDash dashboard;

	Card windSpeed;
	Card windGust;
	Card windDirection;
	Card totalRain;
	Card temperature;
	Card humidity;
	Card heatIndex;
	Card atmPressure;

	WeatherCard();
	void updateCards(WeatherInfo KuyaKim);
};

WeatherCard::WeatherCard()
	: server(80),
		dashboard(&server),
		windSpeed(&dashboard, GENERIC_CARD, "Wind Speed", "km/h"), 
		windGust(&dashboard, GENERIC_CARD, "Wind Gust", "km/h"), 
		windDirection(&dashboard, GENERIC_CARD, "Wind Direction", "°"), 
		totalRain(&dashboard, GENERIC_CARD, "Total Rain", "mm/h"), 
		temperature(&dashboard, TEMPERATURE_CARD, "Temperature", "°C"), 
		humidity(&dashboard, HUMIDITY_CARD, "Humidity", "%"), 
		heatIndex(&dashboard, GENERIC_CARD, "Heat Index", "°C"), 
		atmPressure(&dashboard, GENERIC_CARD, "Atm Pressure", "hPa")
{
	WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
	server.begin();
}

void WeatherCard::updateCards(WeatherInfo KuyaKim)
{
	windSpeed.update(KuyaKim.getWindSpeed());
  windGust.update(KuyaKim.getWindGust());
  windDirection.update(KuyaKim.getWindDirection());
  totalRain.update(KuyaKim.getTotalRain());
  temperature.update(KuyaKim.getTemperature());
  humidity.update(KuyaKim.getHumidity());
  heatIndex.update(KuyaKim.getHeatIndex());
  atmPressure.update(KuyaKim.getAtmPressure());

  dashboard.sendUpdates();
}

#endif