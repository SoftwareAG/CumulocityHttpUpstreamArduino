
#ifndef HttpUpstream_h
#define HttpUpstream_h

#include "Arduino.h"
#include <Client.h>
#include <Base64.h>
#include <ArduinoJson.h>
#include <string.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <EEPROM.h>

class HttpUpstreamClient
{

private:
  char *_clientId;
  char *_host;
  char *_deviceCredentials;
  char *_deviceID;
  Client *_networkClient;

  int storeDeviceCredentialsAndHost(char *host, const char *tenantId, const char *username, const char *password);
  int storeDeviceID();
  int loadDeviceCredentialsAndHostFromEEPROM();
  int requestDeviceCredentialsFromTenant(char *host);
  int loadDeviceIDFromEEPROM();
  int registerDeviceWithTenant(char *deviceName);
  int sendMeasurement(char *body);

public:
  HttpUpstreamClient(Client &networkClient);

  int registerDevice(char *host, char *deviceName);
  int registerDevice(char *host, char *deviceName, char *supportedOperations[]);

  void removeDevice();
  void removeDevice(bool forceClearEEPROM);

  int sendMeasurement(char *type, char *fragment, char *series, int value);
  int sendMeasurement(char *type, char *fragment, char *series, int value, char *unit);
  int sendMeasurement(char *type, char *fragment, char *series, float value);
  int sendMeasurement(char *type, char *fragment, char *series, float value, char *unit);

  void sendAlarm(char *alarm_Type, char *alarm_Text, char *severity);

  void sendEvent(char *event_Type, char *event_Text);
};

#endif
