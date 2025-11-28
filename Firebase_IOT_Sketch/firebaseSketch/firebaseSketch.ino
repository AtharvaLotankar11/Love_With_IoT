#include <FirebaseESP8266.h> // Install Firebase ESP8266 library
#include <ESP8266WiFi.h>
#include <DHT.h>        // Install DHT11 Library and Adafruit Unified Sensor Library

#define WIFI_SSID "Atharva"
#define WIFI_PASSWORD "czvy7706"

#define DHTPIN 2        // Connect Data pin of DHT to D2
int led = D5;          // Connect LED to D5

#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define Firebase configuration and authentication
FirebaseConfig config;
FirebaseAuth auth; // Create FirebaseAuth object
FirebaseData firebaseData;
FirebaseData ledData;

FirebaseJson json;

void setup() {
  Serial.begin(9600);

  dht.begin();
  pinMode(led, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  // Timeout if WiFi doesn't connect within 30 seconds
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime >= 30000) {
      Serial.println("Failed to connect to Wi-Fi within 30 seconds");
      return; // Exit if WiFi connection fails
    }
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set Firebase configuration
  config.host = "iotproject-4623e-default-rtdb.firebaseio.com";  // Correct Firebase URL
  config.api_key = "UyCnRr6jxOhsOhyQHSMtH4ItdjOfJTXY1PQ0dDlp"; 

  // Initialize the Firebase library with the config and auth
  Firebase.begin(&config, &auth);

  // Reconnect Wi-Fi if connection to Firebase is lost
  Firebase.reconnectWiFi(true);
}

void sensorUpdate() {
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.println(F("°C")); // Display temperature in Celsius

  // Send data to Firebase
  if (Firebase.setFloat(firebaseData, "https://iotproject-4623e-default-rtdb.firebaseio.com/IoTProject/temperature", t)) {
    Serial.println("Temperature data sent successfully!");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
  } else {
    Serial.println("FAILED to send temperature");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
  }

  if (Firebase.setFloat(firebaseData, "https://iotproject-4623e-default-rtdb.firebaseio.com/IoTProject/humidity", h)) {
    Serial.println("Humidity data sent successfully!");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
  } else {
    Serial.println("FAILED to send humidity");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
  }
}

void loop() {
  sensorUpdate();

  if (Firebase.getString(ledData, "https://iotproject-4623e-default-rtdb.firebaseio.com/IoTProject/led")) {
    String ledState = ledData.stringData();
    Serial.print("LED State: ");
    Serial.println(ledState);
    
    if (ledState == "1") {
      digitalWrite(led, HIGH);
    } else if (ledState == "0") {
      digitalWrite(led, LOW);
    } else {
      Serial.println("Invalid LED state received!");
    }
  } else {
    Serial.println("Failed to get LED state from Firebase");
    Serial.println("REASON: " + ledData.errorReason());
  }
  
  delay(1000);  // Increase the delay between sensor readings to reduce Firebase load
}
