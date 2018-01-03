// 
// 
// 

#include "ESP8266ThingSpeak.h"
#include <SoftwareSerial.h>

//is being used for monitoring Serial line with computer during trouble shooting.
SoftwareSerial ESP8266(10, 11);//RX,TX

#define API_KEY "J9372YWS8RKCXIWO"
String ApiEndpoint = "184.106.153.149";
String Base = "GET /update?api_key=PBYP9SAE447AVT24&field"; // you channel will have unique code

bool ESP8266ThingSpeakClass::init(String ssid, String pwd)
{
	Serial.begin(115200); // this is to start serial monitoring with your Arduino IDE Serial monitor.
	ESP8266.begin(115200); // this is to start serial communication with the ESP via Software Serial.
	SendCMD("AT+RST");
	delay(2000);

	SendCMD("AT");
	delay(5000);

	// no confirmation connection
	SendCMD("AT+CWJAP=\"" + ssid + "\",\"" + pwd);
	delay(1000);
	SendCMD("AT+CIPSTART=\"TCP\",\"" + ApiEndpoint + "\",80");
	delay(5000);
	SendHealthSignal();

	while (ESP8266.available()) {
		Serial.write(ESP8266.read());
	}

	if (ESP8266.find("OK") || ESP8266.find("WIFI CONNECTED")) {
		Serial.println("OK");
		SendCMD("AT+CWJAP=\"" + ssid + "\",\"" + pwd);
		delay(1000);
		if (ESP8266.find("WIFI GOT IP")) {
			Serial.println("Wifi connected.");
			SendCMD("AT+CIPSTART=\"TCP\",\"" + ApiEndpoint + "\",80");
			delay(5000);

			if (ESP8266.find("Error")) {
				Serial.println("AT+CIPSTART Error");
				return false;
			}
			else {
				Serial.println("Connected to ThingSpeak.");
			}

			SendHealthSignal();
			return true;
		}
		else {
			Serial.println("Wifi not connected.");
			return false;
		}
	}
	else {
		Serial.println("NOT OK");
		return false;
	}
}

void ESP8266ThingSpeakClass::SendCMD(String cmd)
{
	ESP8266.println(cmd); // this resets the ESP8266.
	Serial.println(cmd); // this is just echoing the command to your serial monitor so you can
}

void ESP8266ThingSpeakClass::SendHealthSignal()
{
	String cmd = Base + "3=1" + "\r\n\r\n";
	SendCMD("AT+CIPSEND=" + cmd.length());
	delay(5000);
	if (ESP8266.find(">")) { // if ESP8266 return the > prompt back from the CIPSEND command, you are set, if not something is wrong
		SendCMD(cmd);
	}
	else
	{
		//Serial.println("AT+CIPCLOSE"); //this command somehow does not work and always return errors so I just comment it out
		Serial.println("AT+CIPSEND error");
	}
}


ESP8266ThingSpeakClass ESP8266ThingSpeak;

