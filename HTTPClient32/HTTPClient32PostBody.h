#pragma once

#include <Arduino.h>
#include <WiFiClient.h>

#include "HTTPClient32.h"

#define HTTPCLIENT32_CONTENTSMAX		20


class HTTPClient32;

class HTTPClient32PostBodyContent;
class HTTPClient32PostDataBodyContent;
class HTTPClient32PostMultipartContent;

class HTTPClient32PostBodyString;
class HTTPClient32PostBodyBuf;
class HTTPClient32PostBodyFile;

class HTTPClient32MultipartString;
class HTTPClient32MultipartFileBuffer;
class HTTPClient32MultipartFileFS;

/**
 * @brief POST-body parent
 * 
 */
class HTTPClient32PostBody {
    protected:
        HTTPClient32* client;

    public:
        HTTPClient32PostBody(HTTPClient32* client) {
            this->client = client;
        }

        virtual ~HTTPClient32PostBody() {};

        virtual size_t size() {
            return 0;
        };

        virtual bool send(WiFiClient* stream) {
            return false;
        };
};

/**
 * @brief "Plain" POST-body
 * 
 */
class HTTPClient32PostDataBody: public HTTPClient32PostBody {
    protected:
		HTTPClient32PostBodyContent*		body = NULL;

    public:
        HTTPClient32PostDataBody(HTTPClient32* client): HTTPClient32PostBody(client) {
        }

        ~HTTPClient32PostDataBody() {
            setBody(NULL);
        }

        void setBody(HTTPClient32PostBodyContent* body);

        void setContentType(String &contentType);

        HTTPClient32PostBodyContent* getContent() {
            return this->body;
        }

		HTTPClient32PostDataBody* setBodyString(String contentType, String body);

		HTTPClient32PostDataBody* setBodyFileBuf(String name, String filename, String contentType, uint8_t* buf, size_t buflen);

		HTTPClient32PostDataBody* setBodyFileFS(String name, String filename, String contentType, File &file);

        virtual size_t size();

        virtual bool send(WiFiClient* stream);
};


/**
 * @brief x-www-form-urlencoded POST-body
 * 
 */
class HTTPClient32PostUrlencodedBody: public HTTPClient32PostBody {
    protected:
		size_t  contentSize = 0;
		String  content;

    public:
        HTTPClient32PostUrlencodedBody(HTTPClient32* client);

        ~HTTPClient32PostUrlencodedBody() {
			clear();
        }

		void clear();

        /**
         * @brief URI-encode and add to POST body
         * 
         * @param name 
         * @param value 
         * @return HTTPClient32PostUrlencodedBody* 
         */
		HTTPClient32PostUrlencodedBody* addValue(String name, String value);

        virtual size_t size();

        virtual bool send(WiFiClient* stream);
};


/**
 * @brief Multipart POST-body
 * 
 */
class HTTPClient32PostMultipartBody: public HTTPClient32PostBody {
    protected:
		String                  		    boundaryId;
		uint8_t                 		    contents = 0;
		HTTPClient32PostMultipartContent*	content[HTTPCLIENT32_CONTENTSMAX];

        void generateBoundary();

    public:
        HTTPClient32PostMultipartBody(HTTPClient32* client);

		String getBoundaryId(bool full = true) {
			return String(full ? F("--") : F("")) + boundaryId;
		}

        ~HTTPClient32PostMultipartBody() {
            DEBUGLN("Free multipart body content");
			clear();
        }

		void clear();

        // Add multipart body content unit (false - limit reached)
		bool addContent(HTTPClient32PostMultipartContent* part);

		HTTPClient32MultipartString* addContentString(String name, String value);

		HTTPClient32MultipartString* addContentStringEncoded(String name, String value);

		HTTPClient32MultipartFileBuffer* addContentFileBuf(String name, String filename, String contentType, uint8_t* buf, size_t buflen);

		HTTPClient32MultipartFileFS* addContentFileFS(String name, String filename, String contentType, File &file);

        virtual size_t size();

        virtual bool send(WiFiClient* stream);
};