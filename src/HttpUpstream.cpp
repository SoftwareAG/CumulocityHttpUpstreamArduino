#include "HttpUpstream.h"
#include <WiFi.h>

// Implementations notes
//
// We use C string formatting for constructing JSON bodies instead of something like
// ArduinoJSON, because
// * We could not figure out how to use ArduinoJSON,
//   more specifically, how to allocate exactly the required amount of memory
// * it is efficient

HttpUpstreamClient::HttpUpstreamClient(Client &networkClient)
{
  _networkClient = &networkClient;
}

// Base64 encoder
void HttpUpstreamClient::storeCredentials(char *username, char *password)
{
  char temp[strlen(username) +
            1 + // ":"
            strlen(password) +
            1 // string terminator
  ];

  strcpy(temp, username);
  strcat(temp, ":");
  strcat(temp, password);

#if defined(ARDUINO_ARCH_ESP32)
  String encodedString = base64::encode(temp);

  // Memory allocation
  if (_credentials)
    free(_credentials);
  _credentials = (char *)malloc(sizeof(char) * encodedString.length() + 1);
  strcpy(_credentials, encodedString.c_str());
#else
  int inputStringLength = strlen(temp);
  int encodedLength = Base64.encodedLength(inputStringLength);
  char encodedString[encodedLength];
  Base64.encode(encodedString, temp, inputStringLength);

  // Memory allocation
  if (_credentials)
    free(_credentials);
  _credentials = (char *)malloc(sizeof(char) * strlen(encodedString));
  strcpy(_credentials, encodedString);
#endif
}

// Register device on the cloud and obtain the device id
void HttpUpstreamClient::registerDevice(char *deviceName, char *URL, char *username, char *password)
{
  Serial.println("Preparing to register device.");
  storeCredentials(username, password);
  String test = "{}";

  // JSON Body
  DynamicJsonDocument body(
      JSON_OBJECT_SIZE(2) +
      strlen(deviceName) + 1 +
      test.length());
  String body2send = "";
  body["name"] = deviceName;
  body["c8y_IsDevice"] = "{}";
  serializeJson(body, body2send);

  // HTTP header
  if (_networkClient->connected())
    _networkClient->stop();
  if (_networkClient->connect(URL, 443))
  {
    Serial.println("Registering device...");
    _networkClient->println("POST /inventory/managedObjects/ HTTP/1.1");
    _networkClient->print("Host: ");
    _networkClient->println(URL);
    _networkClient->print("Authorization: Basic ");
    _networkClient->println(_credentials);
    _networkClient->println("Content-Type: application/json");
    _networkClient->print("Content-Length: ");
    _networkClient->println(body2send.length());
    _networkClient->println("Accept: application/json");
    _networkClient->println();
    _networkClient->println(body2send);
    _networkClient->flush();
  }
  else
  {
    Serial.println("Not successful");
  }

  // Device ID
  while (_deviceID.length() == 0)
  {
    String msg = "";
    while (_networkClient->available())
    {
      char c = _networkClient->read();
      msg += c;
    }
    int start = msg.indexOf("\"id\"");
    int until = msg.indexOf("\":", start);
    int until_n = msg.indexOf("\",", until);
    if (until != -1 && start != -1)
    {
      _deviceID = msg.substring(until + 3, until_n);
      Serial.print("Device ID for ");
      Serial.print(deviceName);
      Serial.print(" is ");
      Serial.println(_deviceID);
    }
  }
}

// Measurement Type: c8y_Typemeasuremnt
// Measuremnt Name: c8y_measurmentname
void HttpUpstreamClient::sendMeasurement(int value, char *unit, String timestamp, char *c8y_measurementType, char *c8y_measurementObjectName, char *Name, char *host)
{
  Serial.print("Preparing to send measurement with device ID: ");
  Serial.println(_deviceID);

  int contentLength =
      strlen(Name) +
      strlen(c8y_measurementObjectName) +
      strlen(unit) +
      String(value).length() +
      _deviceID.length() +
      timestamp.length() +
      strlen(c8y_measurementType) +
      69 + // length of template string without placeholders
      1;   // string terminator
  char body2send[contentLength];
  snprintf_P(body2send, contentLength, PSTR("{\"%s\":{\"%s\":{\"unit\":\"%s\",\"value\":%i}},\"source\":{\"id\":\"%s\"},\"time\":\"%s\",\"type\":\"%s\"}"), Name, c8y_measurementObjectName, unit, value, _deviceID.c_str(), timestamp.c_str(), c8y_measurementType);

  if (_deviceID.length() != 0)
  {
    if (_networkClient->connected())
      _networkClient->stop();
    if (_networkClient->connect(host, 443))
    {
      Serial.println("Sending measurement...");

      _networkClient->println("POST /measurement/measurements HTTP/1.1");
      _networkClient->print("Host: ");
      _networkClient->println(host);
      _networkClient->print("Authorization: Basic ");
      _networkClient->println(_credentials);
      _networkClient->println("Content-Type: application/json");
      _networkClient->print("Content-Length: ");
      _networkClient->println(contentLength);
      _networkClient->println("Accept: application/json");
      _networkClient->println();
      _networkClient->println(body2send);
      _networkClient->flush();
    }
    else
    {
      Serial.print("Could not connect to ");
      Serial.println(host);
      Serial.println(WiFi.status());
    }
  }
  else
  {
    Serial.println("Device id undefined. Did you register the device?");
  }
}

void HttpUpstreamClient::sendAlarm(char *alarm_Type, char *alarm_Text, char *severity, String timestamp, char *URL)
{
  Serial.print("Preparing to send alarm device with ID: ");
  Serial.println(_deviceID);

  int contentLength =
      strlen(severity) +
      _deviceID.length() +
      strlen(alarm_Text) +
      timestamp.length() +
      strlen(alarm_Type) +
      64 + // length of template string without placeholders
      1;   // string terminator
  char body2send[contentLength];
  snprintf_P(body2send, contentLength, PSTR("{\"severity\":\"%s\",\"source\":{\"id\":\"%s\"},\"text\":\"%s\",\"time\":\"%s\",\"type\":\"%s\"}"), severity, _deviceID.c_str(), alarm_Text, timestamp.c_str(), alarm_Type);

  if (_deviceID.length() != 0)
  {
    if (_networkClient->connected())
      _networkClient->stop();
    if (_networkClient->connect(URL, 443))
    {
      Serial.println("Sending alarm...");

      _networkClient->println("POST /alarm/alarms HTTP/1.1");
      _networkClient->print("Host: ");
      _networkClient->println(URL);
      _networkClient->print("Authorization: Basic ");
      _networkClient->println(_credentials);
      _networkClient->println("Content-Type: application/json");
      _networkClient->print("Content-Length: ");
      _networkClient->println(contentLength);
      _networkClient->println("Accept: application/json");
      _networkClient->println();
      _networkClient->println(body2send);
      _networkClient->flush();
    }
  }
}

void HttpUpstreamClient::sendEvent(char *event_Type, char *event_Text, String timestamp, char *URL)
{
  Serial.print("Preparing to send event device with ID: ");
  Serial.println(_deviceID);

  int contentLength =
      _deviceID.length() +
      strlen(event_Text) +
      timestamp.length() +
      strlen(event_Type) +
      50 + // length of template string without placeholders
      1;     // string terminator
  char body2send[contentLength];
  snprintf_P(body2send, contentLength, PSTR("{\"source\":{\"id\":\"%s\"},\"text\":\"%s\",\"time\":\"%s\",\"type\":\"%s\"}"), _deviceID.c_str(), event_Text, timestamp.c_str(), event_Type);
  Serial.println(body2send);

  if (_deviceID.length() != 0)
  {
    if (_networkClient->connected())
      _networkClient->stop();
    if (_networkClient->connect(URL, 443))
    {
      Serial.println("Sending event...");

      _networkClient->println("POST /event/events HTTP/1.1");
      _networkClient->print("Host: ");
      _networkClient->println(URL);
      _networkClient->print("Authorization: Basic ");
      _networkClient->println(_credentials);
      _networkClient->println("Content-Type: application/json");
      _networkClient->print("Content-Length: ");
      _networkClient->println(contentLength);
      _networkClient->println("Accept: application/json");
      _networkClient->println();
      _networkClient->println(body2send);
    }
  }
}
