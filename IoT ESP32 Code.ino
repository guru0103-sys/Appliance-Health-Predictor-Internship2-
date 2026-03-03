#include <WiFi.h>
#include "ThingSpeak.h"
#include <DHT.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_NAME";         // Your WiFi Name
const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi Password
unsigned long myChannelNumber = 0000000;     // Your ThingSpeak Channel ID
const char * myWriteAPIKey = "YOUR_API_KEY"; // Your ThingSpeak Write API Key

// --- Pin Definitions ---
#define DHTPIN 4          // DHT11 Data Pin
#define DHTTYPE DHT11     
#define VIB_PIN 5         // SW-420 Signal Pin
#define BUZZER_PIN 18     // 3-Pin Buzzer I/O Pin

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  
  // Initialize Pins
  pinMode(VIB_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off initially
  
  dht.begin();
  
  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  ThingSpeak.begin(client);
}

void loop() {
  // 1. Read Sensor Data
  float temp = dht.readTemperature();
  int vibration = digitalRead(VIB_PIN); // 1 = Vibration detected, 0 = Stable

  // Check if readings are valid
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 2. Local Alert Logic (Buzzer)
  // Trigger alarm if Temp > 45C OR Vibration is detected
  if (temp > 45.0 || vibration == 1) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("ALARM: Anomaly Detected!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 3. Print to Serial Monitor for Debugging
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" C | Vibration: "); Serial.println(vibration);

  // 4. Update ThingSpeak Fields
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, vibration);

  // 5. Push to Cloud
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Wait 20 seconds before next update (ThingSpeak limit)
  delay(20000); 
}