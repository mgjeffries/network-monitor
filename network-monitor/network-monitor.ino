#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager

// Heartbeat settings
const char* heartbeatURL = "https://uptime.betterstack.com/api/v1/heartbeat/Byrp35GjYTtiTSb55ZFeQLJq";
const unsigned long interval = 5 * 60 * 1000;  // 5 minutes in ms
unsigned long previousMillis = 0;

// Config mode button
#define CONFIG_BUTTON_PIN 35


void configModeCallback (WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
  
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }

void setup() {
  pinMode(CONFIG_BUTTON_PIN, INPUT);  // GPIO35 is input only (no pullup)
  delay(100);  // Let things stabilize
  Serial.begin(115200);

  bool configMode = digitalRead(CONFIG_BUTTON_PIN) == LOW;

  WiFiManager wm;
  // wm.setConfigPortalTimeout(180);  // Optional: exit config mode after 3 minutes
  wifiManager.setAPCallback(configModeCallback);

  if (configMode) {
    Serial.println("Entering WiFi config mode...");
    if (!wm.startConfigPortal("ESP32_Setup")) {
      Serial.println("Config portal failed or timed out. Restarting...");
      ESP.restart();
    }
  } else {
    if (!wm.autoConnect("ESP32_Setup")) {
      Serial.println("AutoConnect failed. Restarting...");
      ESP.restart();
    }
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
        Serial.printf("Heartbeat sent, response code: %d\n", httpCode);
      } else {
        Serial.printf("Heartbeat failed: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("WiFi disconnected, skipping heartbeat");
    }
  }
}
