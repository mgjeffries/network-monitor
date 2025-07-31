#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <Pushbutton.h>


// Heartbeat settings
const char* heartbeatURL = "https://uptime.betterstack.com/api/v1/heartbeat/Byrp35GjYTtiTSb55ZFeQLJq";
const unsigned long interval = 5 * 60 * 1000;  // 5 minutes in ms
unsigned long previousMillis = 0;

// Config mode button
Pushbutton button(D9);


void configModeCallback (WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
  
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }

void setup() {
  delay(100);  // Let things stabilize
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected at setup!");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (button.getSingleDebouncedRelease())
  {
    Serial.println("Config mode triggered by pin");

    WiFiManager wm;    

    //reset settings - for testing
    wm.resetSettings();
  
    // set configportal timeout
    wm.setConfigPortalTimeout(180);
    wm.setAPCallback(configModeCallback);

    if (!wm.startConfigPortal("floatyInternetMonitor")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    Serial.println("connected...yay! :)");

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
