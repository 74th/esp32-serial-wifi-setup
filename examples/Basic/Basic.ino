#include <ESP32SerialWiFiSetup.h>

Esp32SerialWifiSetup::WiFiSetupManager wifiSetup;

void setup()
{
    // Initialize the WiFi setup manager
    wifiSetup.begin();

    Serial.println("ESP32 Serial WiFi Setup - JSON-RPC Interface");
    Serial.println("============================================");
    Serial.println("Send JSON-RPC commands to configure WiFi:");
    Serial.println();
    Serial.println("Set WiFi credentials:");
    Serial.println("{\"jsonrpc\": \"2.0\", \"method\": \"set_wifi_creds\", \"params\": {\"ssid\": \"your_ssid\", \"pass\": \"your_password\"}, \"id\":1}");
    Serial.println();
    Serial.println("Get WiFi credentials:");
    Serial.println("{\"jsonrpc\": \"2.0\", \"method\": \"get_wifi_creds\", \"id\":1}");
    Serial.println();
    Serial.println("Get IP address:");
    Serial.println("{\"jsonrpc\": \"2.0\", \"method\": \"get_ip\", \"id\":1}");
    Serial.println();
    Serial.println("Get MAC address:");
    Serial.println("{\"jsonrpc\": \"2.0\", \"method\": \"get_mac_address\", \"id\":1}");
    Serial.println();
}

void loop()
{
    // Handle JSON-RPC commands from serial port
    wifiSetup.handleSerialCommands();

    // Optional: Print connection status every 30 seconds
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 30000)
    {
        lastStatusPrint = millis();

        if (wifiSetup.isConnected())
        {
            Serial.print("Status: Connected to WiFi - IP: ");
            Serial.println(wifiSetup.getIPAddress());
        }
        else
        {
            Serial.println("Status: Not connected to WiFi");
        }
    }
}
