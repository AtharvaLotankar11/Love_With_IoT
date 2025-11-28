#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTTYPE DHT11

const char* ssid = "Sarena_305";
const char* password = "sarena_305";

ESP8266WebServer server(80);

// DHT Sensor
uint8_t DHTPin = D1;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float Temperature;
float Humidity;

void setup() {
  Serial.begin(9600);
  delay(100);

  //LED
  pinMode(D5, OUTPUT);

  dht.begin();

  Serial.println("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  // Connect to your local Wi-Fi network
  WiFi.begin(ssid, password);

  // Wait for Wi-Fi connection with a timeout of 10 seconds
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime >= 10000) {  // Timeout after 10 seconds
      Serial.println("Failed to connect to Wi-Fi. Check your network or credentials.");
      return;
    }
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  digitalWrite(D5, HIGH);
  delay(1000);
  digitalWrite(D5, LOW);
  delay(1000);

  server.handleClient();
}

void handle_OnConnect() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();

  if (isnan(Temperature) || isnan(Humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    server.send(200, "text/html", SendHTML(-1, -1)); // Send -1 to indicate error
  } else {
    server.send(200, "text/html", SendHTML(Temperature, Humidity));
  }
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat, float Humiditystat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 Weather Report</title>\n";
  ptr += "<style>\n";
  ptr += "body {\n";
  ptr += "  background-image: linear-gradient(to right, #FF7E5F, #FEB47B); /* Soft sunset gradient */\n";
  ptr += "  font-family: Arial, sans-serif;\n";
  ptr += "  text-align: center;\n";
  ptr += "  margin: 0;\n";
  ptr += "  padding: 0;\n";
  ptr += "}\n";
  ptr += ".container {\n";
  ptr += "  max-width: 400px;\n";
  ptr += "  margin: 40px auto;\n";
  ptr += "  background: rgba(255, 255, 255, 0.8);\n";
  ptr += "  padding: 20px;\n";
  ptr += "  border-radius: 10px;\n";
  ptr += "  box-shadow: 0 0 15px rgba(0, 0, 0, 0.3);\n";
  ptr += "}\n";
  ptr += ".header {\n";
  ptr += "  color: #444;\n";
  ptr += "  margin-bottom: 20px;\n";
  ptr += "}\n";
  ptr += ".data {\n";
  ptr += "  font-size: 24px;\n";
  ptr += "  color: #444;\n";
  ptr += "  margin-bottom: 10px;\n";
  ptr += "}\n";
  ptr += ".footer {\n";
  ptr += "  margin-top: 30px;\n";
  ptr += "  font-size: 16px;\n";
  ptr += "  color: #444;\n";
  ptr += "  font-style: italic;\n";
  ptr += "  text-align: center;\n";
  ptr += "}\n";
  ptr += ".weather-icon {\n";
  ptr += "  width: 100px;\n"; // Adjust width for better fit
  ptr += "  height: 100px;\n"; // Adjust height for better fit
  ptr += "  margin-bottom: 20px;\n"; // Add some space below the icon
  ptr += "}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div class=\"container\">\n";

  // Using the provided image URL
  ptr += "<img src=\"https://img.icons8.com/?size=100&id=17442&format=png&color=000000\" class=\"weather-icon\">\n";

  ptr += "<h1 class=\"header\">ESP8266 NodeMCU Weather Report</h1>\n";
  if (Temperaturestat == -1 || Humiditystat == -1) {
    ptr += "<p class=\"data\">Error reading sensor data! Please check the sensor connections.</p>\n";
  } else {
    ptr += "<p class=\"data\">Temperature: " + String((int)Temperaturestat) + "°C</p>\n";
    ptr += "<p class=\"data\">Humidity: " + String((int)Humiditystat) + "%</p>\n";
  }
  ptr += "</div>\n";
  ptr += "<div class=\"footer\">\n";
  ptr += "<p>Atharva Lotankar, Harsh Duggar</p>\n";
  ptr += "<p>Manaswi Kabadi, Saniya Kadam</p>\n";
  ptr += "<p>(D10C - Batch B)</p>\n";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
