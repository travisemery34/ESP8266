#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <aREST.h>
#include <DHT.h>

#define DHTPIN 5 // GPIO5 == D1 on the ESP board.
#define DHTTYPE DHT11 // DHT11 is the model sensor I'm using

const char* ssid     = "SKYNET";
const char* password = "killermike";

//Node static IP
IPAddress ip(192,168,1,128);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Values read from sensor
float humidity, temp_f;


DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);
String webString="";     // String to display
aREST rest = aREST();


void setupWebServer(){
  Serial.println("Entering: setupWebServer()");
  server.on("/", [](){  // if you add this subdirectory to your webserver call, you get text below :)

    webString = "{\"temperature\": "+String((float)temp_f);// Arduino has a hard time with float to string
    webString+= ",\"humidity\": "+String((int)humidity)+"}";

    server.send(200, "application/json", webString); // send to someones browser when asked
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
  WiFi.config(ip, gateway, subnet);

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
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  temp_f = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(temp_f, humidity);
}


void setup() {
  Serial.begin(9600);
  //Serial.println("Waking...");
  initWifi();
  dht.begin();
  readAmbiance();
  setupWebServer();
}

void loop() {
  server.handleClient();
  //Serial.println("Sleeping...");
  //ESP.deepSleep(60000000);
}
