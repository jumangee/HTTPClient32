#include "HTTPClient32Headers.h"
#include "HTTPClient32Client.h"

HTTPClient32Headers* HTTPClient32Headers::set(String name, String value) {
	Header* header = find(name);
	if (!header) {
		if (headersCount >= HTTPCLIENT32_HEADERSMAX) {
			return this;
		}
		header = new Header();
		header->name = name;
		this->headers[headersCount] = header;
		headersCount++;
	}
	header->value = value;
	return this;
}

String HTTPClient32Headers::get(String name) {
	Header* h = find(name);
            if (h) {
                return h->value;
            }
            return String(F(""));
}

HTTPClient32Headers::Header* HTTPClient32Headers::find(String name) {
    for (uint8_t i = 0; i < headersCount; i++) {
				Header* h = headers[i];
				if (h->name.equalsIgnoreCase(name)) {
					return h;
				}
			}
			return NULL;
}

HTTPClient32Headers* HTTPClient32Headers::clear() {
	for (uint8_t i = 0; i < headersCount; i++) {
		delete headers[i];
	}
	headersCount = 0;
	return this;
}

bool HTTPClient32Headers::is(String name) {
	return (find(name) != NULL);
}

uint8_t HTTPClient32Headers::size() {
    return headersCount;
}

HTTPClient32Headers::Header* HTTPClient32Headers::get(uint8_t pos) {
    if (pos >= headersCount) {
        return NULL;
    }
    return headers[pos];
}
