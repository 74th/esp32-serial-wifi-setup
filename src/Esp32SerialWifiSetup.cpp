#include "Esp32SerialWifiSetup.h"

namespace Esp32SerialWifiSetup
{

    WiFiSetupManager::WiFiSetupManager()
        : _timeout(30000), _ssid(""), _password("")
    {
    }

    void WiFiSetupManager::begin(unsigned long timeout_ms)
    {
        _timeout = timeout_ms;
        Serial.begin(115200);
        delay(100);

        Serial.println("ESP32 Serial WiFi Setup Library");
        Serial.println("==============================");
    }

    bool WiFiSetupManager::setupWiFi()
    {
        // Check if already connected
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Already connected to WiFi");
            _printWiFiStatus();
            return true;
        }

        Serial.println("\nStarting WiFi setup...");
        Serial.println("Available networks:");
        _scanNetworks();

        Serial.println("\nEnter WiFi credentials:");
        Serial.print("SSID: ");

        // Wait for SSID input
        while (!Serial.available())
        {
            delay(100);
        }
        _ssid = Serial.readStringUntil('\n');
        _ssid.trim();

        Serial.print("Password: ");

        // Wait for password input
        while (!Serial.available())
        {
            delay(100);
        }
        _password = Serial.readStringUntil('\n');
        _password.trim();

        Serial.println("\nConnecting to WiFi...");

        WiFi.begin(_ssid.c_str(), _password.c_str());

        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < _timeout)
        {
            delay(500);
            Serial.print(".");
        }

        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("WiFi connected successfully!");
            _printWiFiStatus();
            return true;
        }
        else
        {
            Serial.println("Failed to connect to WiFi");
            return false;
        }
    }

    bool WiFiSetupManager::isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    String WiFiSetupManager::getSSID()
    {
        return WiFi.SSID();
    }

    String WiFiSetupManager::getIPAddress()
    {
        return WiFi.localIP().toString();
    }

    void WiFiSetupManager::resetWiFi()
    {
        WiFi.disconnect();
        _ssid = "";
        _password = "";
        Serial.println("WiFi credentials reset. Please run setupWiFi() again.");
    }

    bool WiFiSetupManager::_waitForSerialInput()
    {
        unsigned long startTime = millis();
        while (!Serial.available() && (millis() - startTime) < _timeout)
        {
            delay(100);
        }
        return Serial.available();
    }

    void WiFiSetupManager::_printWiFiStatus()
    {
        Serial.println("WiFi Status:");
        Serial.print("  SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("  IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("  Signal Strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    }

    void WiFiSetupManager::_scanNetworks()
    {
#if defined(ESP32)
        int n = WiFi.scanNetworks();

        if (n == 0)
        {
            Serial.println("No networks found");
        }
        else
        {
            for (int i = 0; i < n; ++i)
            {
                Serial.print("  ");
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.print(" dBm)");
                if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN)
                {
                    Serial.print(" *");
                }
                Serial.println();
            }
        }
        WiFi.scanDelete();
#endif
    }

} // namespace Esp32SerialWifiSetup
