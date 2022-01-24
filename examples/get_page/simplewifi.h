#pragma once

#include "Arduino.h"
#include "WiFi.h"

const char* SECRET_NETWORK_SSID = "WIFI1";
const char* SECRET_NETWORK_PASS = "xxx";

bool connectToWiFi() {
	WiFi.mode(WIFI_STA);

	WiFi.begin(SECRET_NETWORK_SSID, SECRET_NETWORK_PASS);
	long int StartTime = millis();
	while (WiFi.status() != WL_CONNECTED)	{
		delay(500);
		if ((StartTime + 10000) < millis())
			break;
	}

	if (WiFi.status() == WL_CONNECTED) {
		return true;
	}
	
	return false;
}