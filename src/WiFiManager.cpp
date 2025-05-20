#include <WiFi.h>
#include "Secrets.h"

class WiFiManager {
public:
    void connectToWiFi() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        Serial.print("Connecting to WiFi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
        Serial.println("\nConnected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    void disconnect() {
        WiFi.disconnect();
        Serial.println("Disconnected from WiFi");
    }
};