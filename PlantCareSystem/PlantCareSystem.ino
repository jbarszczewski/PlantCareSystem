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
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

#define ONE_WIRE_BUS 13
#define LIGHTS_PIN 8
#define HUMIDITY_PIN A1

const float MIN_HOUR = 6;
const float MAX_HOUR = 22;
const float HUMIDITY_RESOLUTION = 1024;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int LightsState = LOW;
int DeviceCount;

//ESP8266ThingSpeakClass thingspeak;
LiquidCrystal lcd(7, 6, 2, 3, 4, 5);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	pinMode(LIGHTS_PIN, OUTPUT);
	//thingspeak.init(SSID, PWD);
	lcd.begin(16, 2);
	sensors.begin();
	lcd.print("Initializing...");
	delay(1000);

	//setTime();

	DeviceCount = sensors.getDeviceCount();
}

// the loop function runs over and over again until power down or reset
void loop()
{
	sensors.requestTemperatures();
	float currentTemp = sensors.getTempCByIndex(0);
	float humidityFactor = (HUMIDITY_RESOLUTION - analogRead(HUMIDITY_PIN)) / HUMIDITY_RESOLUTION;

	if (hour() > MAX_HOUR)
		LightsState = LOW;
	else if (hour() < MIN_HOUR)
		LightsState = HIGH;
	digitalWrite(LIGHTS_PIN, LightsState);

	// print the temp and humidity
	// lcd.clear();
	// lcd.print("T: ");
	// lcd.print(currentTemp);
	// lcd.print(" C");
	// lcd.setCursor(0, 1);
	// lcd.print("H: ");
	// lcd.print(humidityFactor);

	// print time and control status
	lcd.clear();
	printTime();

	lcd.setCursor(0, 1);
	lcd.print("Lights ");
	if (LightsState)
		lcd.print("ON");
	else
		lcd.print("OFF");
	delay(1000);
}

void printDigits(int digits)
{
	if (digits < 10)
		lcd.print('0');
	lcd.print(digits);
}

void printTime()
{
	tmElements_t tm;

	if (RTC.read(tm))
	{
		lcd.print(tm.Hour);
		lcd.print(':');
		lcd.print(tm.Minute);
		lcd.print(':');
		lcd.print(tm.Second);
		lcd.print(' ');
		lcd.print(tm.Day);
		lcd.print('/');
		lcd.print(tm.Month);
		lcd.print('/');
		lcd.print(tmYearToCalendar(tm.Year));
	}
	else
	{
		if (RTC.chipPresent())
		{
			lcd.print("DS1307 is stopped. Run setTime");
		}
		else
		{
			lcd.print("DS1307 read error!");
		}
	}
}

void setTime()
{
	tmElements_t tm;
	int Hour, Min, Sec;

	if (sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
		Serial.println("No time");
	else
	{
		tm.Hour = Hour;
		tm.Minute = Min;
		tm.Second = Sec;
		Serial.println("Time!");
	}

	char Month[12];
	int Day, Year;
	uint8_t monthIndex;

	if (sscanf(__DATE__, "%s %d %d", Month, &Day, &Year) != 3)
		Serial.println("No date");
	else
	{
		for (monthIndex = 0; monthIndex < 12; monthIndex++)
		{
			if (strcmp(Month, monthName[monthIndex]) == 0)
				break;
		}
		if (monthIndex >= 12)
			Serial.println("No date 2");
		else
		{
			tm.Day = Day;
			tm.Month = monthIndex + 1;
			tm.Year = CalendarYrToTm(Year);
			Serial.println("Date");
		}
	}

	if (RTC.write(tm))
	{
		Serial.println("Set");		
	}
	else
		Serial.println("Not set");
}