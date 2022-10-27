#include "HttpUpstream.h"


HttpUpstreamClient::HttpUpstreamClient(Client& networkClient){

 _networkClient = &networkClient;

  }


//Base64 encoder
void HttpUpstreamClient::base64(char* username, char* password){
  char temp[100];
  
  strcpy(temp , username);
  strcat(temp, ":");
  strcat(temp, password);
  Serial.println();
  //For debugging
  Serial.print("Information needs to be coded is:");
  Serial.println(temp);
  
  int inputStringLength = strlen(temp);
  int encodedLength = Base64.encodedLength(inputStringLength);
  char encodedString[encodedLength];
  Base64.encode(encodedString, temp, inputStringLength);
  Serial.print("Encoded string is: ");
  Serial.println(encodedString);

  if (_base64) free (_base64);
  _base64 = (char*) malloc(sizeof(char) * strlen(encodedString));
  strcpy(_base64, encodedString);
  
  Serial.print("Stored Based64 string is: ");
  Serial.println(_base64);
  }


//Register device on the cloud and obtain the device id
void HttpUpstreamClient::registerDevice(char* deviceName, char* URL, char* username, char* password){

 //Base64 encoding
 base64(username, password);

 //JSON Body
 StaticJsonDocument<150> body;
 String body2send= "";
 body["name"] = deviceName;
 body["c8y_IsDevice"]="{}";
 //serializeJsonPretty(body, Serial);  
 serializeJsonPretty(body, body2send);
 //Serial.println(body2send); //comment this line out for debugging

 //HTTP header
if(_networkClient->connect(URL,443)){
    Serial.println("Connected to server");
    //Serial.println(_base64);
    _networkClient->println("POST /inventory/managedObjects/ HTTP/1.1");
    _networkClient->print("Host: ");
    _networkClient->println(URL);
    _networkClient->print("Authorization: Basic ");
    _networkClient->println(_base64);
    _networkClient->println("Content-Type: application/json");
    _networkClient->print("Content-Length: ");
    _networkClient->println(body2send.length());
    _networkClient->println("Accept: application/json");
    _networkClient->println();
    _networkClient->println(body2send);  
  
    }else{
     Serial.println("Not successful"); 
      } 

   //Device ID 
   while(_deviceID.length()==0){
        String msg="";
       while(_networkClient->available()){
         char c = _networkClient->read();
         msg += c;
       }
         int start = msg.indexOf("\"id\"");
         int until = msg.indexOf("\":", start);
         int until_n = msg.indexOf("\",",until);
         if(until!= -1 && start!= -1 ){
         _deviceID = msg.substring(until+3,until_n);
         Serial.print("Device ID for ");
         Serial.print(deviceName);
         Serial.print(" is :");
         Serial.println(_deviceID);
      
        }  
    
    }
  
  }

  //Send measurement value to the cloud
  //Measurement Type: c8y_Typemeasuremnt
  //Measuremnt Name: c8y_measurmentname 
  void HttpUpstreamClient::sendMeasurement(int value, char* unit, String timestamp,char* c8y_measurementType,char* c8y_measurementObjectName,char* Name,char* URL){
    
    Serial.print("The measurement is sent to the device: ");
    Serial.println(_deviceID);

    StaticJsonDocument<350> root;
    JsonObject source  = root.createNestedObject("source");
 
    JsonObject c8y_Measureobject  = root.createNestedObject(c8y_measurementObjectName);
    JsonObject ObjectName = c8y_Measureobject.createNestedObject(Name);
    
    source["id"] = _deviceID;
    root["time"] = timestamp;
    root["type"] = c8y_measurementType;
    ObjectName["unit"] = unit;
    ObjectName["value"] = value;
    
    String body2send= "";
    serializeJsonPretty(root, body2send);
    //Serial.println(body2send);  //comment this line out for debugging
      
    if(_deviceID.length()!=0){
    
    if(_networkClient->connect(URL,443)){
    Serial.println("Connected to server");
    
    _networkClient->println("POST /measurement/measurements HTTP/1.1");  
    _networkClient->print("Host: ");
    _networkClient->println(URL);
    _networkClient->print("Authorization: Basic ");
    _networkClient->println(_base64);
    _networkClient->println("Content-Type: application/json");
    _networkClient->print("Content-Length: ");
    _networkClient->println(body2send.length());
    _networkClient->println("Accept: application/json");
    _networkClient->println();
    _networkClient->println(body2send);
    }                                                
        }
        
  }


    
  void HttpUpstreamClient::sendAlarm(char* alarm_Type, char* alarm_Text, char* severity,String timestamp, char* URL){
    
    Serial.print("The alarm is sent to the device: ");
    Serial.println(_deviceID);

    StaticJsonDocument<150> root;
    JsonObject source  = root.createNestedObject("source");
    source["id"] = _deviceID;
    root["time"] = timestamp;
    root["text"] = alarm_Text;
    root["type"] = alarm_Type;
    root["severity"] = severity;
    
    String body2send= "";
    serializeJsonPretty(root, body2send);
    //Serial.println(body2send); //comment this line out for debugging  
      
    if(_deviceID.length()!=0){
    
    if(_networkClient->connect(URL,443)){
    Serial.println("Connected to server");
    
    _networkClient->println("POST /alarm/alarms HTTP/1.1");  
    _networkClient->print("Host: ");
    _networkClient->println(URL);
    _networkClient->print("Authorization: Basic ");
    _networkClient->println(_base64);
    _networkClient->println("Content-Type: application/json");
    _networkClient->print("Content-Length: ");
    _networkClient->println(body2send.length());
    _networkClient->println("Accept: application/json");
    _networkClient->println();
    _networkClient->println(body2send);
    }                                                
        }
        
  }


  void HttpUpstreamClient::sendEvent(char* event_Type, char* event_Text, String timestamp, char* URL){
    
    Serial.print("The event is sent to the device: ");
    Serial.println(_deviceID);

    StaticJsonDocument<150> root;
    JsonObject source  = root.createNestedObject("source");
    source["id"] = _deviceID;
    root["time"] = timestamp;
    root["text"] = event_Text;
    root["type"] = event_Type;
    
    
    String body2send= "";
    serializeJsonPretty(root, body2send);
    //Serial.println(body2send);  //comment this line out for debugging
      
    if(_deviceID.length()!=0){
    
    if(_networkClient->connect(URL,443)){
    Serial.println("Connected to server");
    
    _networkClient->println("POST /event/events HTTP/1.1");  
    _networkClient->print("Host: ");
    _networkClient->println(URL);
    _networkClient->print("Authorization: Basic ");
    _networkClient->println(_base64);
    _networkClient->println("Content-Type: application/json");
    _networkClient->print("Content-Length: ");
    _networkClient->println(body2send.length());
    _networkClient->println("Accept: application/json");
    _networkClient->println();
    _networkClient->println(body2send);
    }                                                
        }
        
  }
