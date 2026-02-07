#include "config.h"
#include "wifi_handler.h"

void setupWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();

    ssl_client.setInsecure();
    ssl_client.setTimeout(1000);
    ssl_client.setHandshakeTimeout(5);
}