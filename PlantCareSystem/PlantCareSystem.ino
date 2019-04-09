/*
 Name:		PlantCareSystem.ino
 Created:	6/12/2016 4:05:54 PM
 Author:	jbarszczewski
*/

#include <SoftwareSerial.h>
#include "Secrets.h"
#include <stdlib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <elapsedMillis.h>
//#include "ESP8266ThingSpeak.h"

#define LIGHTS_PIN 8
#define WATER_PIN 9
#define BUTTON_1 12
#define ONE_WIRE_BUS 13
#define HUMIDITY_PIN A1

const int MIN_HOUR = 6;
const int MAX_HOUR = 22;
const int WATERING_STEPS = 4;
const int WATER_TIME_HOURS[WATERING_STEPS] = {6, 10, 16, 22};
const int WATER_ON_MILLISECONDS = 30000;
const float HUMIDITY_RESOLUTION = 1024;
const bool LightOnState = true;
const bool WaterOnState = false;

const char *monthName[12] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int LightsState = !LightOnState;
int WaterState = !WaterOnState;
int WateringIndex = 0;
elapsedMillis waterTimer;

//ESP8266ThingSpeakClass thingspeak;
LiquidCrystal lcd(7, 6, 2, 3, 4, 5);

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	pinMode(LIGHTS_PIN, OUTPUT);
	pinMode(WATER_PIN, OUTPUT);
	pinMode(BUTTON_1, INPUT);
	//thingspeak.init(SSID, PWD);
	lcd.begin(16, 2);
	lcd.print("Initializing...");
	delay(1000);

	//	if (digitalRead(BUTTON_1))
	setTime();

	tmElements_t tmTemp;

	if (RTC.read(tmTemp))
	{
		while (tmTemp.Hour > WATER_TIME_HOURS[WateringIndex])
		{
			WateringIndex++;
		}
	}
}

// the loop function runs over and over again until power down or reset
void loop()
{
	// print time and control status
	lcd.clear();
	tmElements_t tm;

	if (RTC.read(tm))
	{
		print2Digits(tm.Hour);
		lcd.print(':');
		print2Digits(tm.Minute);
		lcd.print(':');
		print2Digits(tm.Second);
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

	//print watering index
	lcd.print(" WS: ");
	//	lcd.print(waterTimer / 1000);
	lcd.print(WateringIndex);

	lcd.setCursor(0, 1);
	lcd.print("L: ");

	// if lights off
	if (tm.Hour >= MAX_HOUR || tm.Hour < MIN_HOUR)
	{
		LightsState = !LightOnState;
		lcd.print("OFF");
	}
	// if ligts on
	else if (tm.Hour >= MIN_HOUR && tm.Hour < MAX_HOUR)
	{
		LightsState = LightOnState;
		lcd.print("ON");
	}

	lcd.print(" W: ");
	if (WaterState == WaterOnState)
	{
		lcd.print("ON");
		if (waterTimer > WATER_ON_MILLISECONDS)
		{
			WaterState = !WaterOnState;
			waterTimer = 0;
		}
	}
	else
	{
		lcd.print("OFF");
		if (tm.Hour == WATER_TIME_HOURS[WateringIndex])
		{
			WaterState = WaterOnState;
			waterTimer = 0;
			WateringIndex++;
		}
	}

	if (WateringIndex == WATERING_STEPS)
	{
		WateringIndex = 0;
	}

	digitalWrite(LIGHTS_PIN, LightsState);
	digitalWrite(WATER_PIN, WaterState);

	delay(500);
}

void print2Digits(int number)
{
	if (number < 10)
		lcd.print('0');
	lcd.print(number);
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