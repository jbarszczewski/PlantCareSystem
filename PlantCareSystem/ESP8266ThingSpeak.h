// ESP8266ThingSpeak.h

#ifndef _ESP8266THINGSPEAK_h
#define _ESP8266THINGSPEAK_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class ESP8266ThingSpeakClass
{
protected:

public:
	bool init(String, String);
	void SendCMD(String cmd);
	void SendHealthSignal();
};

extern ESP8266ThingSpeakClass ESP8266ThingSpeak;

#endif

