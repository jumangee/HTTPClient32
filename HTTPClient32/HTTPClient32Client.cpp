#include "HTTPClient32Client.h"
#include "HTTPClient32PostBody.h"
#include "HTTPClient32Response.h"
#include "uri_encode.h"
#include "HTTPClient32Headers.h"

HTTPClient32::HTTPClient32() {
	this->requestHeaders = new HTTPClient32Headers();
}

void HTTPClient32::clear() {
	setBody(NULL);
	setRequest(NULL);
	setResponse(NULL);
}

 HTTPClient32::~HTTPClient32() {
	clear();
	if (this->client) {
		delete client;
	}
	delete requestHeaders;
}

HTTPClient32Headers* HTTPClient32::getRequestHeaders() {
	return this->requestHeaders;
}

HTTPClient32Headers* HTTPClient32::getResponseHeaders() {
	if (!this->response) {
		return NULL;
	}
	return this->response->getHeaders();
}

void HTTPClient32::setBody(HTTPClient32PostBody* body) {
	if (this->body) {
		delete this->body;
	}
	this->body = body;
}

HTTPClient32PostDataBody* HTTPClient32::setPostBody() {
	HTTPClient32PostDataBody* body = new HTTPClient32PostDataBody(this);
	this->setBody(body);
	return body;
}

HTTPClient32PostUrlencodedBody* HTTPClient32::setPostEncodedBody() {
	HTTPClient32PostUrlencodedBody* body = new HTTPClient32PostUrlencodedBody(this);
	this->setBody(body);
	return body;
}

HTTPClient32PostMultipartBody* HTTPClient32::setPostMultipartBody() {
	HTTPClient32PostMultipartBody* body = new HTTPClient32PostMultipartBody(this);
	this->setBody(body);
	return body;
}

HTTPClient32* HTTPClient32::setAuth(String auth) {
	this->getRequestHeaders()->set(F("Authorization"), auth);
	return this;
}

HTTPClient32* HTTPClient32::setAuthBasicLogin(String username, String pwd) {
	return this->setAuth(base64::encode(username + ':' + pwd));
}

HTTPClient32* HTTPClient32::setAuthBearer(String token) {
	return this->setAuth(String(F("BEARER ")) + token);
}

bool HTTPClient32::GET(String host, uint16_t port, String uri, bool secure, const char* rootCACertificate) {
	setBody(NULL);
	
	if (secure) {
		prepareHTTPS(METHOD_TYPE::HTTP_GET, host, port, uri, rootCACertificate);
	} else {
		prepareHTTP(METHOD_TYPE::HTTP_GET, host, port, uri);
	}
	return HTTP();
}

bool HTTPClient32::GET(String host, uint16_t port, String uri) {
	return GET(host, port, uri, false, NULL);
}

bool HTTPClient32::POST(String host, uint16_t port, String uri) {
	return POST(host, port, uri, false, NULL);
}

bool HTTPClient32::POST(String host, uint16_t port, String uri, bool secure, const char* rootCACertificate) {
	if (!body) {
		DEBUGLN("POST content is empty");
		return false;
	}
	if (secure) {
		prepareHTTPS(METHOD_TYPE::HTTP_POST, host, port, uri, rootCACertificate);
	} else {
		prepareHTTP(METHOD_TYPE::HTTP_POST, host, port, uri);
	}
	return HTTP();
}		

HTTPClient32* HTTPClient32::prepareHTTP(METHOD_TYPE method, String host, uint16_t port, String uri) {
	setRequest(new HTTPRequest(false, method, host, port, uri));
	return this;
}

bool HTTPClient32::HTTP() {
	if (!this->request) {
		DEBUGLN("No request target prepared");
		return false;
	}
	if (this->request->secure) {
		WiFiClientSecure* c = new WiFiClientSecure();
		if (this->request->rootCACertificate) {
			c->setCACert(this->request->rootCACertificate);
		}
		client = c;
	} else {
		client = new WiFiClient();
	}
	bool res = send();
	client->stop();
	delete client;
	client = NULL;
	
	return res;
}

void HTTPClient32::setResponse(HTTPClient32Response* r) {
	if (response) {
		delete this->response;
	}
	this->response = r;
}

HTTPClient32ResponseString* HTTPClient32::setResponseToString() {
	HTTPClient32ResponseString* result = new HTTPClient32ResponseString();
	setResponse(result);
            return result;
}

HTTPClient32ResponseFile* HTTPClient32::setResponseToFile(File &file) {
            HTTPClient32ResponseFile* result = new HTTPClient32ResponseFile(file);
	setResponse(result);
            return result;
}

HTTPClient32ResponsePrint* HTTPClient32::setResponseToPrint(Print* print) {
            HTTPClient32ResponsePrint* result = new HTTPClient32ResponsePrint(print);
	setResponse(result);
            return result;
}

HTTPClient32Response* HTTPClient32::getResponse() {
	return this->response;
}

String HTTPClient32::getResponseString() {
	 return ((HTTPClient32ResponseString*)this->getResponse())->result;
}

String HTTPClient32::URIEncode(String src) {
            size_t dst_size = uri_encode_buffer_size(src.c_str(), src.length());
            char dst[dst_size];
            uri_encode(src.c_str(), src.length(), dst, dst_size);
            
	String result = String(dst);
	result.replace(' ', '+');
            return result;
        }

void HTTPClient32::sendHeader(String name, String value) {
	String buf = name + String(": ") + value;
	client->println(buf);
	DEBUGLN(buf);
}

bool HTTPClient32::send() {
	if (!response) {
		setResponseToString();
	}
	if (!client->connect(this->request->host.c_str(), this->request->port)) {
		DEBUGLN("Couldnt connect!");
		this->response->onError(REQUEST_ERROR::NO_CONNECTION);
		return false;
	}
	String buf = this->request->method + F(" ") + this->request->uri + String(" HTTP/1.1");
	DEBUGLN(buf);
	client->println(buf);
	
	sendHeader(F("Host"), this->request->host);

	for (uint8_t i = 0; i < requestHeaders->size(); i++) {
		HTTPClient32Headers::Header* h = requestHeaders->get(i);
		sendHeader(h->name, h->value);
	}
	if (this->request->methodType == METHOD_TYPE::HTTP_POST) {
		if (body) {
			DEBUGLN("Sending POST size");
			sendHeader(String(HTTPCLIENT32_HEADER_CONTENTLENGTH), String(body->size()));
			DEBUGLN("Headers sent");
			DEBUGLN();
			client->println();
			body->send(this->client);
		} else {
			DEBUGLN("ERROR! Body is empty!");
		}
	} else {
		DEBUGLN("Headers sent");
		DEBUGLN();
		client->println();
	}
	DEBUGLN("Send complete");
	
	if (handleHeaderResponse()) {
		return true;
	}
	return false;
}

bool HTTPClient32::handleHeaderResponse() 
{
	uint16_t returnCode = 0;
	int _size = -1;
	unsigned long lastDataTime = millis();
	unsigned long responseStarted = lastDataTime;
	bool firstLine = true;
	HTTPClient32Headers* responseHeaders = this->response->getHeaders();
	while(client->connected()) {
		size_t len = client->available();
		if(len > 0) {
			String headerLine = client->readStringUntil('\n');
			headerLine.trim(); // remove \r
			lastDataTime = millis();
			DEBUGLN(headerLine);
			if (firstLine) {
				firstLine = false;
				int codePos = headerLine.indexOf(' ') + 1;
				returnCode = headerLine.substring(codePos, headerLine.indexOf(' ', codePos)).toInt();
			} else if(headerLine.indexOf(':')) {
				String headerName = headerLine.substring(0, headerLine.indexOf(':'));
				String headerValue = headerLine.substring(headerLine.indexOf(':') + 1);
				headerValue.trim();
				if (headerName.equalsIgnoreCase(HTTPCLIENT32_HEADER_CONTENTLENGTH)) {
					_size = headerValue.toInt();
				}
				if (headerName.length() > 0) {
					responseHeaders->set(headerName, headerValue);
				}
			}
			if (headerLine == "") {
				DEBUG("code: ");
				DEBUGLN(returnCode);
				this->response->setReturnCode(returnCode);
				if(_size > 0) {
					this->response->expected(_size);
					DEBUG("size: ");
					DEBUGLN(_size);
				}
				if(returnCode) {
					handleResponseBody(_size, responseStarted);
					return true;
				}
				DEBUGLN("Remote host is not an HTTP Server!");
				this->response->onError(REQUEST_ERROR::NOT_A_WEBSERVER);
				return false;
			}
		} else {
			if((millis() - responseStarted) > this->request->responseTimeout) {
				DEBUGLN("Response (HEAD) timeout!");
				this->response->onError(REQUEST_ERROR::NO_RESPONSE);
				return false;
			}
			delay(10);
		}
	}
	DEBUGLN("CONNECTION_LOST");
	this->response->onError(CONNECTION_LOST);
	return false;
}

void HTTPClient32::handleResponseBody(size_t expectedSize, unsigned long responseStarted) {
	size_t bodySize = 0;
	//unsigned long start = millis();
	while (client->connected()) {
		if ((millis() - responseStarted) >= this->request->responseTimeout) {
			DEBUGLN("Response (READ) timeout!");
			this->response->onError(REQUEST_ERROR::READTIMEOUT);
			return;
		}
		while (client->available()) {
			this->response->print(client->read());
			bodySize++;
		}
		if (bodySize > 0 && bodySize >= expectedSize) {
			return;
		}
	}
}		

void HTTPClient32::setRequest(HTTPRequest* request) {
	if (this->request) {
		delete this->request;
	}
	this->request = request;
}
