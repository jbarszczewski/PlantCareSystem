/*
 Name:		PlantCareSystem.ino
 Created:	6/12/2016 4:05:54 PM
 Author:	jbarszczewski
*/

#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal.h>

#define ONE_WIRE_BUS 10
#define HEAT_PIN 13

const float MIN_TEMP = 20.0;
const float MAX_TEMP = 26.0;

int HeatState = LOW;
int CursorPosition;
int DeviceCount;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(HEAT_PIN, OUTPUT);
	CursorPosition = 0;
	lcd.begin(16, 2);
	delay(1000);
	sensors.begin();
	DeviceCount = sensors.getDeviceCount();
}

// the loop function runs over and over again until power down or reset
void loop() {
	float currentTemp = sensors.getTempCByIndex(0);

	if (currentTemp > MAX_TEMP)
		HeatState = LOW;
	else if (currentTemp < MIN_TEMP)
		HeatState = HIGH;

	digitalWrite(HEAT_PIN, HeatState);
	// print and log the temp
	lcd.clear();
	lcd.setCursor(CursorPosition, 0);
	lcd.print(DeviceCount);
	CursorPosition++;
	if (CursorPosition == 16)
		CursorPosition = 0;
	lcd.setCursor(0, 1);
	sensors.requestTemperatures();
	lcd.print("Temp: ");
	lcd.print(currentTemp);
	lcd.print(" C");
	delay(200);
}
