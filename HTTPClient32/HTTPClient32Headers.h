#pragma once

#include <Arduino.h>

#include "HTTPClient32.h"

#define HTTPCLIENT32_HEADERSMAX			25

class HTTPClient32;

class HTTPClient32Headers {
    public:
		struct Header {
			String name;
			String value;
		};

		HTTPClient32Headers* set(String name, String value);

		~HTTPClient32Headers() {
			clear();
		}

		String get(String name);

        HTTPClient32Headers::Header* find(String name);

		HTTPClient32Headers* clear();
        
		bool is(String name);

        uint8_t size();

        HTTPClient32Headers::Header* get(uint8_t pos);

    protected:
		Header* headers[HTTPCLIENT32_HEADERSMAX];
		uint8_t headersCount = 0;
};