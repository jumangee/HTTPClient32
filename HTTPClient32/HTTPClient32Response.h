#pragma once

#include <Arduino.h>
#include "FS.h"

#include "HTTPClient32.h"

class HTTPClient32Headers;

class HTTPClient32Response {
    protected:
        HTTPClient32Headers*    headers;
        uint16_t                returnCode = 0;

    public:
        HTTPClient32Response();

        virtual ~HTTPClient32Response();

        HTTPClient32Headers* getHeaders();

        virtual void setReturnCode(uint16_t code);

        uint16_t getReturnCode() {
            return returnCode;
        };

        virtual void onError(REQUEST_ERROR errCode) {};

        virtual void print(char c) = 0;
        virtual void expected(size_t size) {};
};

class HTTPClient32ResponseString: public HTTPClient32Response {
    public:
        String result;

        HTTPClient32ResponseString(): HTTPClient32Response() {
            result = "";
        }

        void expected(size_t size) {
            result.reserve(size);
        }

        void print(char c) {
            result += c;
        }

        String getURIDecoded();
};

class HTTPClient32ResponseFile: public HTTPClient32Response {
    public:
        File file;

        HTTPClient32ResponseFile(File &file): HTTPClient32Response() {
            this->file = file;
        }

        void print(char c) {
            file.print(c);
        }
};

class HTTPClient32ResponsePrint: public HTTPClient32Response {
    public:
        Print* prn;

        HTTPClient32ResponsePrint(Print* print): HTTPClient32Response() {
            this->prn = print;
        }

        void print(char c) {
            prn->print(c);
        }
};
