#include "HTTPClient32PostBody.h"
#include "HTTPClient32Headers.h"

void HTTPClient32PostDataBody::setBody(HTTPClient32PostBodyContent* body) {
    if (this->body) {
        delete body;
    }
    this->body = body;
}

void HTTPClient32PostDataBody::setContentType(String &contentType) {
    client->getRequestHeaders()->set(HTTPCLIENT32_HEADER_CONTENTTYPE, contentType);
}

HTTPClient32PostDataBody* HTTPClient32PostDataBody::setBodyString(String contentType, String body) {
	HTTPClient32PostBodyString* c = new HTTPClient32PostBodyString(this, contentType, body);
	setBody(c);
	return this;
}

HTTPClient32PostDataBody* HTTPClient32PostDataBody::setBodyFileBuf(String name, String filename, String contentType, uint8_t* buf, size_t buflen) {
	HTTPClient32PostBodyBuf* c = new HTTPClient32PostBodyBuf(this, contentType, buf, buflen);
	setBody(c);
	return this;
}

HTTPClient32PostDataBody* HTTPClient32PostDataBody::setBodyFileFS(String name, String filename, String contentType, File &file) {
	HTTPClient32PostBodyFile* c = new HTTPClient32PostBodyFile(this, contentType, file);
	setBody(c);
	return this;
}

size_t HTTPClient32PostDataBody::size() {
    return this->body->getSize();
}

bool HTTPClient32PostDataBody::send(WiFiClient* stream) {
    this->body->send(stream);
    return true;
}

HTTPClient32PostUrlencodedBody::HTTPClient32PostUrlencodedBody(HTTPClient32* client) : HTTPClient32PostBody(client){
    client->getRequestHeaders()->set(HTTPCLIENT32_HEADER_CONTENTTYPE, F("application/x-www-form-urlencoded"));
}

void HTTPClient32PostUrlencodedBody::clear() {
	content = "";
            contentSize = 0;
}

HTTPClient32PostUrlencodedBody* HTTPClient32PostUrlencodedBody::addValue(String name, String value) {
            String encoded = HTTPClient32::URIEncode(value);
            if (contentSize) {
                content += '&';
            }
            content += name + '=' + encoded;
            contentSize = content.length();
	
	return this;
}

size_t HTTPClient32PostUrlencodedBody::size() {
    return contentSize + 2;
}

bool HTTPClient32PostUrlencodedBody::send(WiFiClient* stream) {
    stream->println(content);
    return true;
}

void HTTPClient32PostMultipartBody::generateBoundary() {
    boundaryId.reserve(7 + sizeof(millis()));
			for (uint8_t i = 0; i < 7; i++) {
				boundaryId += String(random(10));
			}
			boundaryId += String(millis());
			
    //DEBUG("boundaryId: ");
			//DEBUGLN(boundaryId);
    String header = F("multipart/form-data; boundary=");
    header += String(boundaryId);   //getBoundaryId(); 
    client->getRequestHeaders()->set(HTTPCLIENT32_HEADER_CONTENTTYPE, header);
}

HTTPClient32PostMultipartBody::HTTPClient32PostMultipartBody(HTTPClient32* client) : HTTPClient32PostBody(client){
    generateBoundary();
}

void HTTPClient32PostMultipartBody::clear() {
	for (uint8_t i = 0; i < contents; i++) {
		delete content[i];
	}
}

bool HTTPClient32PostMultipartBody::addContent(HTTPClient32PostMultipartContent* part) {
	if (contents < HTTPCLIENT32_CONTENTSMAX) {
		content[contents] = part;
		contents++;
		return true;
	}
            delete part;
	return false;
}

HTTPClient32MultipartString* HTTPClient32PostMultipartBody::addContentString(String name, String value) {
	HTTPClient32MultipartString* c = new HTTPClient32MultipartString(this, name, value);
	if (addContent(c)) return c;
	return NULL;
}

HTTPClient32MultipartString* HTTPClient32PostMultipartBody::addContentStringEncoded(String name, String value) {
	HTTPClient32MultipartString* c = new HTTPClient32MultipartString(this, name, HTTPClient32::URIEncode(value));
	if (addContent(c)) return c;
	return NULL;
}

HTTPClient32MultipartFileBuffer* HTTPClient32PostMultipartBody::addContentFileBuf(String name, String filename, String contentType, uint8_t* buf, size_t buflen) {
	HTTPClient32MultipartFileBuffer* c = new HTTPClient32MultipartFileBuffer(this, name, filename, contentType, buf, buflen);
	if (addContent(c)) return c;
	return NULL;
}

HTTPClient32MultipartFileFS* HTTPClient32PostMultipartBody::addContentFileFS(String name, String filename, String contentType, File &file) {
	HTTPClient32MultipartFileFS* c = new HTTPClient32MultipartFileFS(this, name, filename, contentType, file);
	if (addContent(c)) return c;
	return NULL;
}

size_t HTTPClient32PostMultipartBody::size() {
    size_t size = 0;
    for (uint8_t i = 0; i < contents; i++) {
        HTTPClient32PostBodyContent* c = content[i];
        size += c->getSize();
    }
    return size + (getBoundaryId(true)).length() + 2 + 2;
}

bool HTTPClient32PostMultipartBody::send(WiFiClient* stream) {
    DEBUGLN("Sending multipart body");
    for (uint8_t i = 0; i < contents; i++) {
        content[i]->send(stream);
    }
    DEBUGLN(getBoundaryId(true) + F("--"));
    stream->println(getBoundaryId(true) + F("--"));
    return true;
}        
