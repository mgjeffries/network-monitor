#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>

const char* heartbeatURL = "https://uptime.betterstack.com/api/v1/heartbeat/Byrp35GjYTtiTSb55ZFeQLJq";
const unsigned long interval = 5 * 60 * 1000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Create WiFiManager instance
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);
  
  // Automatically connect using saved credentials,
  // or start access point if none found
  if (!wm.autoConnect("ESP32_Setup")) {
    Serial.println("Failed to connect and hit timeout");
    ESP.restart();
  }

  Serial.println("Connected to WiFi!");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(heartbeatURL);
      int httpCode = http.GET();
      
      if (httpCode > 0) {
        Serial.printf("Ping sent, HTTP response code: %d\n", httpCode);
      } else {
        Serial.printf("Ping failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
  }
}
