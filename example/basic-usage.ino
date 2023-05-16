#include <HttpUpstream.h>
#include "arduino_secrets.h" //you can store your WiFi SSID and pass in this file
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFiClientSecure.h>

WiFiClientSecure networkClient;

// Only relevant for ESP32
// cumulocity.com root ca certificate
// The certificate should work for any Cumulocity IoT tenant hosted by Software AG.*
// If it does not, please look here https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure
// and contact us at IoTEP@softwareag.com.
// *(Unless you are using something like mitmproxy in transparent mode. In that case we expect you to know what you are doing.)
const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n"
    "EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n"
    "EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n"
    "ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\n"
    "NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n"
    "EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\n"
    "AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\n"
    "DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\n"
    "E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\n"
    "/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\n"
    "DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\n"
    "GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\n"
    "tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\n"
    "AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\n"
    "FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\n"
    "WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\n"
    "9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\n"
    "gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\n"
    "2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\n"
    "LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\n"
    "4uJEvlz36hz1\n"
    "-----END CERTIFICATE-----\n";
#else
#include <WiFiNINA.h>

WiFiSSLClient networkClient;
#endif

HttpUpstreamClient c8yClient(networkClient);

int status = WL_IDLE_STATUS;

// General Configuration
char *deviceName = "My Arduino";
char *host = "luis-iotep-testing.cumulocity.com";

// Create a file named arduino_secrets.h with content
//
// #define SECRET_SSID "<wifi name>"
// #define SECRET_PASS "<wifi password>"
//
// where you replace <wifi name> and <wifi password> with the name and password of your wifi network respectively.
// This allows sharing your code with less risk of accidentally sharing your credentials than directly specifying them here.
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Used for the sendEvent example
int loopIterations = 0;

// setup runs once when you start the device.
// Put initialization code here.
void setup()
{
  // Sets baud rate for communication via the serial port.
  // It enables you to subsequently log information to the _Serial Monitor_, which can be accessed via the _Tools_ menu in the Arduino editor.
  // Statements like Serial.println("Hello World!") log to the Serial Monitor.
  // You need to select the same baud rate in the Serial Monitor as you specify here.
  // This library includes a number of prints to the Serial Monitor in order to inform you about the current connection status etc.
  // Baud rate specifies the maximum number of bits transferred per second. 9600 should be sufficient for logging as a debug tool.
  Serial.begin(9600);

  // Connects to WiFi
  // Limitation: Only covers happy case. E.g. it does not tell you, when you have entered a wrong password.
  WiFi.begin(ssid, pass);
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    status = WiFi.status();
    Serial.print("Status is ");
    Serial.println(status);

    delay(3000);
  }
  Serial.println("Connected to WiFi");

#if defined(ARDUINO_ARCH_ESP32)
  networkClient.setCACert(root_ca);
#endif
  // Check wether device was already registered or register device and trigger internal setup.
  // I.e. if you are running this for the first time, it registers the device with your tenant.
  // If you are running this a second and subsequent times, it loads device credentials from persistent memory etc.
  // Limitation: It expects that EEPROM - persistent memory - was not tampered with.
  status = c8yClient.registerDevice(host, deviceName);
  // POSIX convention is to return 0 when everything is ok and 1 to 255 for everything else.
  // Because in C false is defined as 0 and true as everything not 0, the POSIX convention is convenient for error handling like this.
  if (status)
  {
    Serial.print("Could not register or resume device: ");
    Serial.println(status);
    while (true)
      ;
  }
}

// loop runs repeatedly after setup was executed once.
// Put main control flow logic here.
void loop()
{
  // Read sensor values.
  // Here we generate a random value, because this depends on your connected sensors.
  float temperature = (float)random(54000) / (float)100;

  // Send measurement.
  c8yClient.sendMeasurement("c8y_TemperatureMeasurement", "c8y_Steam", "T", temperature, "C");

  // Send an alarm, when something occurs, which requires human intervention.
  if (temperature > 490)
  {
    c8yClient.sendAlarm("example-alarm-type", "Steam is very hot right now. Might be due to fast proton.", "WARNING");
  }

  // Send an event, when something occurs, which should be logged and does not require human intervention.
  int messageLength = 25 + String(loopIterations).length();
  char message[messageLength];
  snprintf_P(message, messageLength, PSTR("Executed the loop %i times."), loopIterations);
  c8yClient.sendEvent("example-event-type", message);
  loopIterations++;

  // Delay sending in order to keep network burden as well as consumption on the Cumulocity tenant to a minimum.
  // Sending a lot can be expensive in terms of power and money.
  //
  // delay busy waits, which consumes energy.
  // If power consumption is important to you, you should look into putting the Arduino into sleep mode and waking it from an external source.

  // Note that delay(5000) does not mean that we send every 5s. It means that the loop execution takes 5s longer than without the delay.
  delay(5000);
}