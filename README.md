# HTTPClient32
Flexible and simple object-oriented HTTP client library for the ESP32 supporting GET, POST urlencoded and multipart requests

## Examples

### Simple POST
```
  HTTPClient32* client = new HTTPClient32();
  
	// add some custom headers
  HTTPClient32Headers* headers = client->getRequestHeaders();
	headers
		->set("bee", "123")
		->set("gggg", "asdfadf");

	// set simple post body
	client
		->setPostBody()
		
		// set content with content-type
		->setBodyString("plaint/text", "fuzz=bbbb");

    if (client->POST("quest-book.ru", 80, "/")) {
		Serial.println("Response POST: ");
		Serial.print(client->getResponseString());
    } else {
        Serial.println("ERROR!");
    }
	delete client;
 ```
 
### Multipart POST
```
  HTTPClient32* client = new HTTPClient32();
  
  // request can be sent later
  client->prepareHTTPS(
    HTTP_METHOD_POST,
    F("api.telegram.org"),
    443,
    (String(F("/bot")) + authKey + '/sendMessage'),
    TELEGRAM_CERTIFICATE_ROOT
  );

  HTTPClient32PostMultipartBody* body = client->setPostMultipartBody();
  body->addContentString(F("text"), text);
  body->addContentString(F("chat_id"), chat_id);
  
  if (client->HTTP()) {
    HTTPClient32HeadersContainer* headers = client->getResponseHeaders();
    DEBUGLN("RESPONSE:");
    for (uint8_t i = 0; i < headers->size(); i++) {
        HTTPClient32HeadersContainer::Header* h = headers->get(i);
        DEBUGLN("Response header: " + h->name + "=" + h->value);
      }

    Serial.println(client->getResponseString());
  }
  
	delete client;
 ```
