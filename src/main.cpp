#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN 5 // GPIO5 == D1 on the ESP board.
#define DHTTYPE DHT11 // DHT11 is the model sensor I'm using

const char* ssid     = "ssid";
const char* password = "pwd";
float humidity, temp_f;  // Values read from sensor

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);
String webString="";     // String to display


void setupWebServer(){
  Serial.println("Entering: setupWebServer()");
  server.on("/", [](){  // if you add this subdirectory to your webserver call, you get text below :)

    webString = "<!DOCTYPE html><html><h1>";
    webString+= "Temperature: "+String((int)temp_f)+" *F";// Arduino has a hard time with float to string
    webString+= " Humidity: "+String((int)humidity)+"%";
    webString+= "</h1></html>";

    server.send(200, "text/html", webString); // send to someones browser when asked
  });
  server.begin();
  Serial.println("HTTP server started");
}

void initWifi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void readAmbiance(){
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  temp_f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(temp_f, humidity);

  //These are just debug printouts to console
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temp_f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hif);
  Serial.println(" *F");
}


void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  initWifi();
  dht.begin();
  setupWebServer();
}

void loop() {
  readAmbiance();
  server.handleClient();
}
