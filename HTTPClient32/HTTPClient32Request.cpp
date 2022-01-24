#include "HTTPClient32Request.h"

void HTTPClient32PostBodyContent::setContent(String content, size_t totalSize) {
    this->content = content;
    this->size = totalSize;
};

void HTTPClient32PostBodyContent::sendBuf(WiFiClient* stream, uint8_t* buf, size_t buflen) {
    DEBUGLN("HTTPClient32PostBodyContent::sendBuf size: " + String(buflen));
    for (size_t n = 0; n < buflen; n = n + HTTPCLIENT32REQUEST_PACKETSIZE)	{
        if (n + HTTPCLIENT32REQUEST_PACKETSIZE < buflen)	{
            stream->write(buf, HTTPCLIENT32REQUEST_PACKETSIZE);
            DEBUGLN("written " + String(HTTPCLIENT32REQUEST_PACKETSIZE));
            buf += HTTPCLIENT32REQUEST_PACKETSIZE;
        }
        else if (buflen % HTTPCLIENT32REQUEST_PACKETSIZE > 0)	{
            size_t remainder = buflen % HTTPCLIENT32REQUEST_PACKETSIZE;
            stream->write(buf, remainder);
            DEBUGLN("written " + String(remainder));
        }
    }
    DEBUGLN("HTTPClient32PostBodyContent::sendBuf done");
}

void HTTPClient32PostBodyContent::sendFile(WiFiClient* stream, File &file) {
    DEBUGLN("sendFile: " + String(file.size()));
    size_t len = file.size();
    uint8_t buf[HTTPCLIENT32REQUEST_PACKETSIZE];
    DEBUGLN("sendFile size: " + String(len));
    for (size_t n = 0; n < len; n = n + HTTPCLIENT32REQUEST_PACKETSIZE)	{
        if (n + HTTPCLIENT32REQUEST_PACKETSIZE < len)	{
            file.read(buf, HTTPCLIENT32REQUEST_PACKETSIZE);
            stream->write(buf, HTTPCLIENT32REQUEST_PACKETSIZE);
            DEBUGLN("written " + String(HTTPCLIENT32REQUEST_PACKETSIZE));
        }
        else if (len % HTTPCLIENT32REQUEST_PACKETSIZE > 0)	{
            size_t remainder = len % HTTPCLIENT32REQUEST_PACKETSIZE;
            file.read(buf, remainder);
            stream->write(buf, remainder);
            DEBUGLN("written " + String(remainder));
        }
    }
    DEBUGLN("sendFile done");
}

HTTPClient32PostDataBodyContent::HTTPClient32PostDataBodyContent(HTTPClient32PostDataBody* post) : HTTPClient32PostBodyContent((HTTPClient32PostBody*)post){
}

HTTPClient32PostBodyString::HTTPClient32PostBodyString(HTTPClient32PostDataBody* post, String contentType, String value) : HTTPClient32PostDataBodyContent(post){
    String result = value + EOL;
    post->setContentType(contentType);
    setContent(result, result.length());
}

HTTPClient32PostBodyString::HTTPClient32PostBodyString(HTTPClient32PostDataBody* post) : HTTPClient32PostDataBodyContent(post){
}

HTTPClient32PostBodyBuf::HTTPClient32PostBodyBuf(HTTPClient32PostDataBody* post, String contentType, uint8_t* buf, size_t buflen) : HTTPClient32PostDataBodyContent(post){
    this->buf = buf;
    this->buflen = buflen;
    post->setContentType(contentType);
    setContent("", buflen);
}

HTTPClient32PostBodyFile::HTTPClient32PostBodyFile(HTTPClient32PostDataBody* post, String contentType, File &file) : HTTPClient32PostDataBodyContent(post){
    this->file = file;
    post->setContentType(contentType);
    setContent("", file.size());
}

String HTTPClient32PostMultipartContent::getPartHeader() {
    String result = ((HTTPClient32PostMultipartBody*)postbody)->getBoundaryId();
    result += EOL;
    result += F("Content-Disposition: form-data; name=\"");
    result += this->name;
    result += "\"";
    return result;
}

HTTPClient32PostMultipartContent::HTTPClient32PostMultipartContent(HTTPClient32PostMultipartBody* post, String name) : HTTPClient32PostBodyContent((HTTPClient32PostBody*)post){
    this->name = name;
}

HTTPClient32MultipartString::HTTPClient32MultipartString(HTTPClient32PostMultipartBody* post, String name, String value) : HTTPClient32PostMultipartContent(post, name){
    this->setValue(value);
}

void HTTPClient32MultipartFile::fileContent(String &filename, String &contentType, size_t binarySize) {
    String result = getPartHeader();
    if (filename.length() > 0) {
        result += F("; filename=\"");
        result += filename;
        result += F("\"");
    }
    result += EOL;
    if (contentType.length() > 0) {
        result += F("Content-Type: ");
        result += contentType;
        result += EOL;
    }
    result += EOL;
    setContent(result, result.length() + buflen);
}

HTTPClient32MultipartFileBuffer::HTTPClient32MultipartFileBuffer(HTTPClient32PostMultipartBody* post, String name, String filename, String contentType, uint8_t* buf, size_t buflen) : HTTPClient32MultipartFile(post, name){
    this->setValue(filename, contentType, buf, buflen);
}

HTTPClient32MultipartFileFS::HTTPClient32MultipartFileFS(HTTPClient32PostMultipartBody* post, String name, String filename, String contentType, File &file) : HTTPClient32MultipartFile(post, name){
    this->setValue(filename, contentType, file);
}
