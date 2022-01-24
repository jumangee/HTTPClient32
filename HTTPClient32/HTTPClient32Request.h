#pragma once

#include <WiFiClient.h>
#include "FS.h"

#include "HTTPClient32.h"
#include "uri_encode.h"

class HTTPClient32;
class HTTPClient32PostBody;
class HTTPClient32PostDataBody;
class HTTPClient32PostMultipartBody;
class HTTPClient32PostUrlencodedBody;


#ifndef HTTPCLIENT32REQUEST_PACKETSIZE
    #define HTTPCLIENT32REQUEST_PACKETSIZE 4096
#endif


class HTTPClient32PostBodyContent {
    protected:
        HTTPClient32PostBody*   postbody;
        String	  	            content;
        size_t			        size = 0;

        void setContent(String content, size_t totalSize);

        void sendBuf(WiFiClient* stream, uint8_t* buf, size_t buflen);
        
        void sendFile(WiFiClient* stream, File &file);

    public:
        size_t getSize() {
            return size;
        };

        virtual void send(WiFiClient* stream) {
            DEBUG(content);
            stream->print(content);
        };

        HTTPClient32PostBodyContent(HTTPClient32PostBody* post) {
            this->postbody = post;
        };

        virtual ~HTTPClient32PostBodyContent() {};
};


/*** Simple POST body content ***/

class HTTPClient32PostDataBodyContent: public HTTPClient32PostBodyContent {
    public:
        HTTPClient32PostDataBodyContent(HTTPClient32PostDataBody* post);
};


class HTTPClient32PostBodyString: public HTTPClient32PostDataBodyContent {
    public:
        HTTPClient32PostBodyString(HTTPClient32PostDataBody* post, String contentType, String value);

        HTTPClient32PostBodyString(HTTPClient32PostDataBody* post);
};


class HTTPClient32PostBodyBuf: public HTTPClient32PostDataBodyContent {
    protected:
        uint8_t*  	buf = NULL;
        size_t	  	buflen = 0;
    public:
        virtual void send(WiFiClient* stream) {
            this->sendBuf(stream, this->buf, this->buflen);
        };

        HTTPClient32PostBodyBuf(HTTPClient32PostDataBody* post, String contentType, uint8_t* buf, size_t buflen);
};


class HTTPClient32PostBodyFile: public HTTPClient32PostDataBodyContent {
    protected:
        File	file;

    public:
        virtual void send(WiFiClient* stream) {
            this->sendFile(stream, this->file);
        };

        HTTPClient32PostBodyFile(HTTPClient32PostDataBody* post, String contentType, File &file);
};


/*** Multipart POST body content ***/

class HTTPClient32PostMultipartContent: public HTTPClient32PostBodyContent {
    protected:
        String name;

        String getPartHeader();

    public:
        HTTPClient32PostMultipartContent(HTTPClient32PostMultipartBody* post, String name);
};


class HTTPClient32MultipartString: public HTTPClient32PostMultipartContent {
    public:
        HTTPClient32MultipartString(HTTPClient32PostMultipartBody* post, String name, String value);

        void setValue(String value) {
            String result = this->getPartHeader();
            result += EOL;
            result += EOL;
            result += value;
            result += EOL;

            setContent(result, result.length());
        }
};


class HTTPClient32MultipartFile: public HTTPClient32PostMultipartContent {
    protected:
        uint8_t*  	buf = NULL;
        size_t	  	buflen = 0;

        void fileContent(String &filename, String &contentType, size_t binarySize);

    public:
        HTTPClient32MultipartFile(HTTPClient32PostMultipartBody* post, String name): HTTPClient32PostMultipartContent(post, name)  {
        }
};


class HTTPClient32MultipartFileBuffer: public HTTPClient32MultipartFile {
    protected:
        uint8_t*  	buf = NULL;
        size_t	  	buflen = 0;

    public:
        virtual void send(WiFiClient* stream) {
            HTTPClient32PostBodyContent::send(stream);
            this->sendBuf(stream, this->buf, this->buflen);
        };

        HTTPClient32MultipartFileBuffer(HTTPClient32PostMultipartBody* post, String name, String filename, String contentType, uint8_t* buf, size_t buflen);

        void setValue(String filename, String contentType, uint8_t* buf, size_t buflen) {
            fileContent(filename, contentType, buflen);

            this->buf = buf;
            this->buflen = buflen;			
        }        
};


class HTTPClient32MultipartFileFS: public HTTPClient32MultipartFile {
    protected:
        File	file;

    public:
        virtual void send(WiFiClient* stream) {
            HTTPClient32PostBodyContent::send(stream);
            this->sendFile(stream, this->file);
        };

        HTTPClient32MultipartFileFS(HTTPClient32PostMultipartBody* post, String name, String filename, String contentType, File &file);

        void setValue(String filename, String contentType, File &file) {
            fileContent(filename, contentType, file.size());

            this->file = file;
        }        

};