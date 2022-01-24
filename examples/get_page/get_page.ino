#include "Arduino.h"
#include "simplewifi.h"
#include <HTTPClient32.h>

#define GETFREERAM DEBUG("--- Free RAM: "); DEBUGLN(ESP.getFreeHeap());

HTTPClient32 *client;

void setup() {
    Serial.begin(115200);

    Serial.println("Starting!");

	if (!connectToWiFi()) {
		DEBUGLN("Could not connect to WIFI!");
	}
}

void loop() {
    DEBUGLN("...");
	client = new HTTPClient32();
	
	// add some custom headers
	HTTPClient32Headers* headers = client->getRequestHeaders();
	headers
		->set("bee", "123")
		->set("gggg", "asdfadf");

    if (client->GET(F("quest-book.ru"), 80, "/")) {
		Serial.println("Response: ");
		Serial.print(client->getResponseString());
    } else {
        Serial.println("ERROR!");
    }
	delete client;

    DEBUGLN("***");
	GETFREERAM
	delay(2000);
}