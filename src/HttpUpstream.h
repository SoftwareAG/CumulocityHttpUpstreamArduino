
#ifndef HttpUpstream_h
#define HttpUpstream_h

#include "Arduino.h"
#include <Client.h>
#include "Base64.h" //Base64 shouldn't be located at library file
#include <ArduinoJson.h>
#include <string.h>

class HttpUpstreamClient{

  private:
  
  char* _clientId;
  char* _host;
  char* _base64;
  String _deviceID;
  
  void base64(char* username, char* password);

  //void obtaindeviceID(String msg);
  
  public:

  Client* _networkClient;
  
  
  HttpUpstreamClient(Client& networkClient);

  //create device with a unique device name  
  void registerDevice(char* deviceName, char* URL, char* username, char* password);

  //send a single measurement to the cloud without knowing the device ID 
  void sendMeasurement(int value, char* unit, String timestamp,char* measurementType,char* measurementObjectName,char* Name,char* URL);

  //send an alarm to the cloud without knowing the device ID 
  void sendAlarm(char* alarm_Type, char* alarm_Text, char* severity,String timestamp, char* URL);

  //send a event to the cloud without knowing the device ID 
  void sendEvent(char* event_Type, char* event_Text, String timestamp, char* URL);
  
  };

#endif
