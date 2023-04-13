
#ifndef HttpUpstream_h
#define HttpUpstream_h

#include "Arduino.h"
#include <Client.h>
#include <Base64.h>
#include <ArduinoJson.h>
#include <string.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class HttpUpstreamClient
{

private:
  char *_clientId;
  char *_host;
  char *_deviceCredentials;
  char * _deviceID;

  int storeDeviceCredentialsAndHost(char* host, const char *tenantId, const char *username, const char *password);
  int storeDeviceID();
  int loadDeviceCredentialsAndHostFromEEPROM();
  int requestDeviceCredentialsFromTenant(char *host);
  int loadDeviceIDFromEEPROM();
  int registerDeviceWithTenant(char *deviceName);

  // void obtaindeviceID(String msg);

public:
  Client *_networkClient;

  HttpUpstreamClient(Client &networkClient);

  // create device with a unique device name
  int registerDevice(char *host, char *deviceName);
  int registerDevice(char *host, char *deviceName, char *supportedOperations[]);

  void removeDevice();
  void removeDevice(bool forceClearEEPROM);

  // send a single measurement to the cloud without knowing the device ID
  void sendMeasurement(int value, char *unit, char *measurementType, char *measurementObjectName, char *Name);

  // send an alarm to the cloud without knowing the device ID
  void sendAlarm(char *alarm_Type, char *alarm_Text, char *severity);

  // send a event to the cloud without knowing the device ID
  void sendEvent(char *event_Type, char *event_Text);
};

#endif
