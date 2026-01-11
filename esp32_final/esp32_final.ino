#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Infinix GT 20 Pro";  // Replace with your Wi-Fi SSID
const char* password = "12345678";   // Replace with your Wi-Fi password

// Pushbullet API details
const String pushbulletApiKey = "-"; // Replace with your Pushbullet API key

// Pushover API details
const String pushoverApiToken = "-"; // Replace with your Pushover API token
const String pushoverUserKey = "-";   // Replace with your Pushover user key

#define RX1_PIN 16 // GPIO16 for RX1
#define TX1_PIN 17 // GPIO17 for TX1

// Set up the WiFi client
WiFiClientSecure client;

// Track Serial1 state
bool wasSerial1DataAvailable = true;

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN); // Initialize Serial1 with appropriate baud rate
  Serial.println("ESP32 is ready to receive data from Mega.");

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to Wi-Fi...");
  }

  // Wi-Fi connected
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if data is received on Serial1
  if (Serial1.available()) {
    String receivedData = Serial1.readStringUntil('\n'); // Read data until newline
    receivedData.trim(); // Remove any extra whitespace
    Serial.println("Received on ESP32: " + receivedData);

    // Reset the flag since we received data
    wasSerial1DataAvailable = true;

    // Check for "ALERT" message and handle it
    if (receivedData.startsWith("ALERT:")) {
      String alertReason = receivedData.substring(7); // Extract reason after "ALERT:"
      Serial.println("Emergency alert received: " + alertReason);

      // Send notification via Pushbullet
      sendPushbulletNotification("Emergency Alert", alertReason);

      // Uncomment the line below to use Pushover instead:
      // sendPushoverNotification("Emergency Alert", alertReason);
    }
  } else {
    // Only print "No data available" once when the state changes
    if (wasSerial1DataAvailable) {
      Serial.println("No data available on Serial1.");
      wasSerial1DataAvailable = false;
    }
  }

  // Add a delay to prevent flooding the Serial Monitor
  delay(1000);

  // Check Wi-Fi periodically
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 5000) { // Every 5 seconds
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
      }
      Serial.println("\nWi-Fi reconnected!");
    }
  }
}

// Function to send Pushbullet notification
void sendPushbulletNotification(String title, String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Prepare the API request URL
    String url = "https://api.pushbullet.com/v2/pushes";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Access-Token", pushbulletApiKey);

    // Create the payload
    String payload = "{\"type\": \"note\", \"title\": \"" + title + "\", \"body\": \"" + message + "\"}";

    // Send the POST request
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Pushbullet response: " + response);
    } else {
      Serial.println("Error sending Pushbullet notification");
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected, cannot send Pushbullet notification");
  }
}

// Function to send Pushover notification
void sendPushoverNotification(String title, String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Prepare the API request URL
    String url = "https://api.pushover.net:443/1/messages.json";
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare the payload
    String payload = "token=" + pushoverApiToken + "&user=" + pushoverUserKey + "&title=" + title + "&message=" + message;

    // Send the POST request
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Pushover response: " + response);
    } else {
      Serial.println("Error sending Pushover notification");
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected, cannot send Pushover notification");
  }
}
