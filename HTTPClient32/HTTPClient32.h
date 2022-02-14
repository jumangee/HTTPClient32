#ifndef HTTPCLIENT32COMMON_H
#define HTTPCLIENT32COMMON_H

#define HTTPMULTIPARTCLIENT_DEBUG_

#define HTTP_METHOD_POST					HTTPClient32::METHOD_TYPE::HTTP_POST
#define HTTP_METHOD_GET						HTTPClient32::METHOD_TYPE::HTTP_GET

#define CONTENTTYPE_JPEG					F("image/jpeg")
#define CONTENTTYPE_PNG						F("image/png")
#define CONTENTTYPE_JSON					F("application/json")
#define CONTENTTYPE_TEXT					F("plain/text")

#define HTTPCLIENT32_HEADER_CONTENTLENGTH	F("Content-Length")
#define HTTPCLIENT32_HEADER_CONTENTTYPE		F("Content-Type")

#define EOL 								F("\r\n")

/***********************************/

#ifdef HTTPMULTIPARTCLIENT_DEBUG
	#define DEBUGLN(t) Serial.println(t)
	#define DEBUG(t) Serial.print(t)
#else
	#define DEBUGLN(t) /**/
	#define DEBUG(t) /**/
#endif

enum REQUEST_ERROR {
	NO_CONNECTION,
	NO_RESPONSE,
	READTIMEOUT,
	NOT_A_WEBSERVER,
	CONNECTION_LOST
};

#include "HTTPClient32Response.h"
#include "HTTPClient32Headers.h"
#include "HTTPClient32Request.h"
#include "HTTPClient32PostBody.h"
#include "HTTPClient32Client.h"

#endif