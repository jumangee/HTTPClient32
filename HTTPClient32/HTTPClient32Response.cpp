#include "HTTPClient32Response.h"
#include "HTTPClient32Headers.h"
#include "uri_encode.h"

HTTPClient32Response::HTTPClient32Response() {
    headers = new HTTPClient32Headers();
}

 HTTPClient32Response::~HTTPClient32Response() {
    delete headers;
}

HTTPClient32Headers* HTTPClient32Response::getHeaders() {
    return headers;
}

void HTTPClient32Response::setReturnCode(uint16_t code) {
    returnCode = code;
};

String HTTPClient32ResponseString::getURIDecoded() {
    size_t dstlen = result.length();
    char decoded[dstlen];
    uri_decode(result.c_str(), (char*)&decoded, dstlen);
    
    return String(decoded);
}
