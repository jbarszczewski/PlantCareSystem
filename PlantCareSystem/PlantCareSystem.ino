/*
 Name:		PlantCareSystem.ino
 Created:	6/12/2016 4:05:54 PM
 Author:	jbarszczewski
*/

#include <SoftwareSerial.h>
//#include "ESP8266ThingSpeak.h"
#include "Secrets.h"
#include <stdlib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal.h>

#define ONE_WIRE_BUS 13         
#define HEAT_PIN 9
#define HUMIDITY_PIN A1

const float MIN_TEMP = 20.0;
const float MAX_TEMP = 26.0;
const float HUMIDITY_RESOLUTION = 1024;

int HeatState = LOW;
int DeviceCount;

//ESP8266ThingSpeakClass thingspeak;
LiquidCrystal lcd(12, 9, 2, 3, 4, 5);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(HEAT_PIN, OUTPUT);
	//thingspeak.init(SSID, PWD);
	lcd.begin(16, 2);
	sensors.begin();
	delay(1000);
	DeviceCount = sensors.getDeviceCount();
}

// the loop function runs over and over again until power down or reset
void loop() {
	float currentTemp = sensors.getTempCByIndex(0);
	int humiditySensorValue = analogRead(HUMIDITY_PIN);
	float  humidityFactor = (HUMIDITY_RESOLUTION - humiditySensorValue) / HUMIDITY_RESOLUTION;
	if (currentTemp > MAX_TEMP)
		HeatState = LOW;
	else if (currentTemp < MIN_TEMP)
		HeatState = HIGH;

	digitalWrite(HEAT_PIN, HeatState);
	// print the temp and humidity
	lcd.clear();
	sensors.requestTemperatures();
	lcd.print("T: ");
	lcd.print(currentTemp);
	lcd.print(" C");
	lcd.setCursor(0, 1);
	lcd.print("H: ");
	lcd.print(humidityFactor);
	//lcd.print(humiditySensorValue);
	delay(1000);
}