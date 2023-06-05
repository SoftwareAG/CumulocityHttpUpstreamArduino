# ur-ArduinoMicrocontroller-library

A library created by Yingzhe and Luis from University Relation, Software AG

This library enables you to post the real-time data, alarm and events to the Cumulocity IoT platform. 

The University Relation created the library which is used for arduino containing the WiFi module WiFiNINA and WiFi101. 

The library was desgined for  MKR WIFI 1010 only!! If you are the user from ESP32 or ESP82, you have to modify the code. Sorry for that!

# Prerequisite

**Libraries to be included**

1. WiFi liraries, e.g., WiFiNINA, WiFi101.
2. NTPClient from https://github.com/taranais/NTPClient

# API
## Register Device

```
void registerDevice(char* deviceName, char* URL, char* username, char* password);
```
- `char* deviceName` Device name 
- `char* URL` The URL where your tenant is. The format should be: *MyTenant.cumulocity.com*
- `char* username` The username of your tenant
- `char* password` The password of your tenant
## Sending measurement 

```
void sendMeasurement(int value, char* unit, String timestamp,char* measurementType,char* measurementObjectName,char* URL);
```
- `int value` Measurement to be sent
- `char* unit` Unit associated with the measurement 
- `String timestamp` Timestamp where the measurement is created
- `char* measurementType` Type of the measurement 
- `char* measurementObjectName` Object Name of the measurement
- `char* URL` The URL where your tenant is. The format should be: *MyTenant.cumulocity.com*
## Sending alarm

```
void sendAlarm(char* alarm_Type, char* alarm_Text, char* severity,String timestamp, char* URL);
```
- `char* alarm_Type` Type of the alarm. The format is, c8y_FollowThisAlarm
- `char* alarm_Text` Text of the alarm. 
- `char* severity` The severity of the alarm. It could be *"CRITICAL"*, *"MAJOR"*, *"MINOR"*, *"WARNING"*
- `String timestamp` Timestamp where the measurement is created
- `char* URL` The URL where your tenant is. The format is: *MyTenant.cumulocity.com*
## Sending alarm
```
void sendEvent(char* event_Type, char* event_Text, String timestamp, char* URL);
```
- `char* event_Type` Type of the event. The format is, c8y_FollowThisEvent
- `char* event_Text` Text of the event.
- `String timestamp` Timestamp where the measurement is created
- `char* URL` The URL where your tenant is. The format is: *MyTenant.cumulocity.com* 


------
These tools are provided as-is and without warranty or support. They do not constitute part of the Software AG product suite. Users are free to use, fork and modify them, subject to the license agreement. While Software AG welcomes contributions, we cannot guarantee to include every contribution in the master project.
