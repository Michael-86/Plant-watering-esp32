#include <WiFi.h>
//#include <WiFiClient.h>
#include <WebServer.h>
//#include <ESPmDNS.h>
#include <Update.h>
#include <millisDelay.h>

#include "mainPage.h"

#include <DHT.h>

// Temp och luftfuktighet
#define DHT_SENSOR_PIN  4 // ESP32 pin GIOP21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// Ljus sensor
#define LIGHT_SENSOR_PIN  39

/* Change these values based on your calibration values */
#define soilWet 1600   // Define max value we consider soil 'wet'
#define soilDry 4000   // Define min value we consider soil 'dry'

// Sensor pins
#define sensorPower 12
#define sensorPin 36

// Relä utgång
const int Load1 = 13;

// Global string
String ledState1 = "OFF";
String ledState2 = "OFF";

// Auto reboot
millisDelay rebootDelay;
unsigned long REBOOT_DELAY_MS = 24ul * 60 * 60 * 1000; // 1day in mS

// Wifi reconnect timer
unsigned long check_wifi = 30000;

// Wifi
const char* host = "esp8266-webupdate";
const char* ssid = "prometheus1";
const char* password = "gharib12";

WebServer server(80);

// Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

// login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" && server.arg("PASSWORD") == "admin") {
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  String content = "<meta charset='utf-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}

// root page can be accessed only if authentification is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  if (!is_authentified()) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  String s = FPSTR(MAIN_page);

  server.send(200, "text/html", s);
}

// no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// OTA HTML
void handleUpdate() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  String content = "<meta charset='utf-8'>"; 
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"; 
  content += "<html><body><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

  server.send(200, "text/html", content);
}

void handleForm() {
}

// HTML knapp för automatik styrning av relä
void handlebutton1() {

  String t_state = server.arg("LEDstate1");  // Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  if (t_state == "1") {
    ledState1 = "ON";          // Feedback parameter
  } 
  else {
    ledState1 = "OFF";          // Feedback parameter
    digitalWrite(Load1, LOW);  // LED OFF
  }

  server.send(200, "text/plane", ledState1);  // Send web page
}

void handlebutton1state() {
  
  server.send(200, "text/plane", ledState1);  // Send web page
}

// HTML Knapp för på/av relä
void handlebutton2() {
  String t_state = server.arg("LEDstate2");  // Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  if (t_state == "1") {
    digitalWrite(Load1, HIGH);  // LED ON
    ledState2 = "ON";          // Feedback parameter

  } else {
    digitalWrite(Load1, LOW);  // LED OFF
    ledState2 = "OFF";          // Feedback parameter

  }

  server.send(200, "text/plane", ledState2);  // Send web page
}

void handlebutton2state() {
  
  server.send(200, "text/plane", ledState2);  // Send web page
}

// Temperatur
void handletempratur() {

  //Serial.println("temp");
  int a = dht_sensor.readTemperature();
  String tempratur = String(a);
  server.send(200, "text/plane", tempratur);  // Send ADC value only to client ajax request
}

// Luftfuktighet
void handleluftfuktighet() {

  //Serial.println("fukt");
  int b = dht_sensor.readHumidity();
  String luftfuktighet = String(b);
  server.send(200, "text/plane", luftfuktighet);  // Send ADC value only to client ajax request
}

// Ljus
void handleljus() {
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  String RAW = String(analogValue);
  String ljusstyrka = String();

  if (analogValue < 40) {
    //Serial.println(" => Dark");
    ljusstyrka = RAW + " => Dark";
  } else if (analogValue < 800) {
    //Serial.println(" => Dim");
    ljusstyrka = RAW + " => Dim";
  } else if (analogValue < 2000) {
    //Serial.println(" => Light");
    ljusstyrka = RAW + " => Light";
  } else if (analogValue < 3200) {
    //Serial.println(" => Bright");
    ljusstyrka = RAW + " => Bright";
  } else {
    //Serial.println(" => Very bright");
    ljusstyrka = RAW + " => Very bright";
  }
  server.send(200, "text/plane", ljusstyrka);  // Send ADC value only to client ajax request
}

// Jordfuktighet
void handlejordfuktighet() {

  int moisture = readSensor();
  String jordfuktighet = String();
	// Determine status of our soil
	if (moisture < soilWet) {
    jordfuktighet = "Status: Soil is too wet";
	} else if (moisture >= soilWet && moisture < soilDry) {
    jordfuktighet = "Status: Soil moisture is perfect";
	} else {
    jordfuktighet = "Status: Soil is too dry - time to water!";
	}
	
  server.send(200, "text/plane", jordfuktighet);  // Send ADC value only to client ajax request

}


void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  digitalWrite(Load1, LOW);
  digitalWrite(sensorPower, LOW);

  pinMode(Load1, OUTPUT);
  pinMode(sensorPower, OUTPUT);

  dht_sensor.begin();

    rebootDelay.start(REBOOT_DELAY_MS); // start reboot timer
  #if defined(ARDUINO_ARCH_ESP32)
    enableLoopWDT(); // default appears to be 5sec
  #elif defined(ARDUINO_ARCH_ESP8266)
    ESP.wdtEnable(5000); // arg ignored :-( default appears to be 3sec
  #else
  #error Only ESP2866 and ESP32 reboot supported
  #endif 

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    //MDNS.begin(host);
    server.on("/", handleRoot);
    server.on("/login", handleLogin);
    server.on("/ota", handleUpdate);
    server.on("/form", handleForm);
    server.on("/button1", handlebutton1);
    server.on("/button1state", handlebutton1state);
    server.on("/button2", handlebutton2);
    server.on("/button2state", handlebutton2state);
    server.on("/readtemp", handletempratur);
    server.on("/readhum", handleluftfuktighet);
    server.on("/readljus", handleljus);
    server.on("/readjordfuktighet", handlejordfuktighet);
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  // here the list of headers to be recorded
  const char* headerkeys[] = { "User-Agent", "Cookie" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  // ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);

    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin()) { // start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { // true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      } else {
        Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
      }
    });
    server.begin();
    //MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
}

void loop(void) {
  server.handleClient();
  delay(2);


  // if wifi is down, try reconnecting every 30 seconds
  if ((WiFi.status() != WL_CONNECTED) && (millis() > check_wifi)) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    check_wifi = millis() + 30000;
  }

  if (rebootDelay.justFinished()) {
    while (1) {} // force watch dog timer reboot
  }

  if (ledState1 == "ON"){
    int moisture = readSensor();

    // Determine status of our soil
    if (moisture < soilWet) {
      digitalWrite(Load1, LOW);
    } else if (moisture >= soilWet && moisture < soilDry) {
      digitalWrite(Load1, LOW);
    } else {
        digitalWrite(Load1, HIGH);
      }
  }
}

int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(1000);							// Allow power to settle
	int val = analogRead(sensorPin);	// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// Return analog moisture value
}
