#include <HttpUpstream.h>
#include "arduino_secrets.h"  //you can store your WiFi SSID and pass in this file
#include <WiFiNINA.h>
#include <NTPClient.h>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


WiFiSSLClient nwc; //initiate WiFiSSL client 
HttpUpstreamClient c8yclient(nwc); // initiate HttpUpstreamClient 


int status = WL_IDLE_STATUS;

//Gereral para
char* deviceName ="ArduinoDevice";
char* URL = "yourtenantname.cumulocity.com";

char* username = "username2logging";
char* password = "yourpassword";


//Measurement
char* measurement_type = "c8y_temperaturemeasurement";
char* measurement_object = "c8y_Steam";
char* object = "temperature";
char* unit = "";



//Alarm Type
char* alarm_Type = "c8y_HellofromArduino";
char* alarm_Text = "Hello World!";
char* severity = "MINOR";

//Event Type
char* event_Type = "c8y_HellofromArduino";
char* event_Text = "This event is used for test";


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE) {
  Serial.println("Communication with WiFi module failed!");
  // don't continue
  while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection:
    delay(6000);
  }
  Serial.println("Connected to WiFi");  
  timeClient.begin();
 
  c8yclient.registerDevice(deviceName,URL,username,password);
  
}

void loop() {
  
  // put your main code here, to run repeatedly:
  // from the server, read them and print them: 
   while (nwc.available()) {
   char c = nwc.read();
   Serial.print(c);        
   }
   
timeClient.update();

//Sending a virtual measurement using a random value
c8yclient.sendMeasurement(random(30),unit,timeClient.getFormattedDate(),measurement_type,measurement_object,object,URL); //Sening realtime measurement

//Sedning an alarm and an event based on a random value
if(random(30) > 15){

c8yclient.sendAlarm(alarm_Type, alarm_Text, severity, timeClient.getFormattedDate(),URL ); //Sening realtime alarm
c8yclient.sendEvent(event_Type, event_Text, timeClient.getFormattedDate(),URL); //Sending realtime event


}



}
