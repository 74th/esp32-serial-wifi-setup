#include <Esp32SerialWifiSetup.h>

Esp32SerialWifiSetup::WiFiSetupManager wifiSetup;

void setup() {
    // Initialize the WiFi setup manager
    wifiSetup.begin();
    
    // Start WiFi setup process
    Serial.println("Starting WiFi setup...");
    Serial.println("Please open Serial Monitor and enter your WiFi credentials.");
    
    if (wifiSetup.setupWiFi()) {
        Serial.println("WiFi setup completed successfully!");
        Serial.print("Connected to: ");
        Serial.println(wifiSetup.getSSID());
        Serial.print("IP Address: ");
        Serial.println(wifiSetup.getIPAddress());
    } else {
        Serial.println("WiFi setup failed. Please try again.");
    }
}

void loop() {
    // Check WiFi connection status every 10 seconds
    static unsigned long lastCheck = 0;
    
    if (millis() - lastCheck > 10000) {
        lastCheck = millis();
        
        if (wifiSetup.isConnected()) {
            Serial.println("WiFi is still connected");
            Serial.print("IP: ");
            Serial.println(wifiSetup.getIPAddress());
        } else {
            Serial.println("WiFi connection lost!");
            // You can call wifiSetup.setupWiFi() again here if needed
        }
    }
    
    delay(1000);
}
