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

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
}

void loop() {
  unsigned long currentMillis = millis();

  if ( digitalRead(CONFIG_BUTTON_PIN) == 1) {
    Serial.println("Config mode triggered by pin");

    WiFiManager wm;    

    //reset settings - for testing
    wm.resetSettings();
  
    // set configportal timeout
    wm.setConfigPortalTimeout(180);
    wm.setAPCallback(configModeCallback);

    if (!wm.startConfigPortal("OnDemandAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

  }

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

  delay(100);  // Let things stabilize

}
