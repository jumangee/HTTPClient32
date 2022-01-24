#pragma once

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <base64.h>

#include "HTTPClient32.h"

class HTTPClient32PostBody;
class HTTPClient32PostDataBody;
class HTTPClient32PostMultipartBody;
class HTTPClient32PostUrlencodedBody;

class HTTPClient32Headers;

class HTTPClient32Response;
class HTTPClient32ResponsePrint;
class HTTPClient32ResponseString;
class HTTPClient32ResponseFile;

class HTTPClient32 {
	public:
		enum METHOD_TYPE {
			HTTP_GET,
			HTTP_POST
		};

		struct HTTPRequest {
			bool		secure;
			String		host;
			String 		uri;
			uint16_t	port;
			String		method;
			uint32_t	responseTimeout = 30000;
			const char* rootCACertificate;

			METHOD_TYPE methodType;

			HTTPRequest(bool secure, HTTPClient32::METHOD_TYPE method, String host, uint16_t port, String uri, const char* rootCACertificate = NULL) {
				this->secure = secure;
				this->methodType = method;
				this->method = HTTP_GET ? F("GET") : F("POST");
				this->host = host;
				this->port = port;
				this->uri = uri;
				this->rootCACertificate = rootCACertificate;
			}
		};
		
		HTTPClient32();

		void clear();

		virtual ~HTTPClient32() {
			clear();
			if (this->client) {
				delete client;
			}
			delete requestHeaders;
		}

		/*** HEADERS ***/

		HTTPClient32Headers* getRequestHeaders();

		HTTPClient32Headers* getResponseHeaders();

		/*** SET POST BODY ***/

		void setBody(HTTPClient32PostBody* body);

		HTTPClient32PostDataBody* setPostBody();

		HTTPClient32PostUrlencodedBody* setPostEncodedBody();

		HTTPClient32PostMultipartBody* setPostMultipartBody();

		/*** END SET POST BODY ***/

		HTTPClient32* setAuth(String auth);

		HTTPClient32* setAuthBasicLogin(String username, String pwd);

		HTTPClient32* setAuthBearer(String token);

		/**
		 * @brief Immediately execute GET-request
		 * 
		 * @param host 
		 * @param port 
		 * @param uri 
		 * @return true 
		 * @return false 
		 */
		bool GET(String host, uint16_t port, String uri, bool secure, const char* rootCACertificate);

		/**
		 * @brief Immediately execute GET-request
		 * 
		 * @param host 
		 * @param port 
		 * @param uri 
		 * @return true 
		 * @return false 
		 */
		bool GET(String host, uint16_t port, String uri);

		/**
		 * @brief Immediately execute POST-request
		 * 
		 * @param host 
		 * @param port 
		 * @param uri 
		 * @return true 
		 * @return false 
		 */
		bool POST(String host, uint16_t port, String uri);

		/**
		 * @brief Immediately execute POST-request
		 * 
		 * @param host 
		 * @param port 
		 * @param uri 
		 * @return true 
		 * @return false 
		 */
		bool POST(String host, uint16_t port, String uri, bool secure, const char* rootCACertificate);

        /**
         * @brief Prepare request for later use
         * 
         * @param method 
         * @param host 
         * @param port 
         * @param uri 
         */
		HTTPClient32* prepareHTTP(METHOD_TYPE method, String host, uint16_t port, String uri);

		HTTPClient32* prepareHTTPS(METHOD_TYPE method, String host, uint16_t port, String uri, const char* rootCACertificate = NULL) {
			setRequest(new HTTPRequest(true, method, host, port, uri, rootCACertificate));
			return this;
		}

		bool setRequestTimeout(uint32_t responseTimeout) {
			if (!this->request) {
				return false;
			}
			this->request->responseTimeout = responseTimeout;
		}

		/**
		 * @brief Execute prepared request
		 * 
		 * @return true 
		 * @return false 
		 */
		bool HTTP();

		void setResponse(HTTPClient32Response* r);

		HTTPClient32ResponseString* setResponseToString();

		HTTPClient32ResponseFile* setResponseToFile(File &file);

		HTTPClient32ResponsePrint* setResponseToPrint(Print* print);

		HTTPClient32Response* getResponse();

		String getResponseString();

		static String URIEncode(String src);

	protected:
		HTTPClient32Headers*	requestHeaders;

		WiFiClient*						client;
		HTTPClient32Response*	response = NULL;
		HTTPClient32PostBody*			body = NULL;
		HTTPRequest*					request = NULL;

		/***/

		void sendHeader(String name, String value);

		/**
		 * @brief Request implementation
		 * 
		 * @return true 
		 * @return false 
		 */
		bool send();

		/**
		 * reads the response from the server
		 * @return bool
		 */
		bool handleHeaderResponse();

		void handleResponseBody(size_t expectedSize, unsigned long responseStarted);

		void setRequest(HTTPRequest* request);
};