#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

namespace Esp32SerialWifiSetup {

class WiFiSetupManager {
public:
    WiFiSetupManager();
    
    /**
     * Initialize the WiFi setup manager
     * @param timeout_ms Timeout for waiting user input (default: 30000ms)
     */
    void begin(unsigned long timeout_ms = 30000);
    
    /**
     * Start WiFi setup process via serial communication
     * @return true if WiFi connected successfully, false otherwise
     */
    bool setupWiFi();
    
    /**
     * Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isConnected();
    
    /**
     * Get current WiFi SSID
     * @return SSID string
     */
    String getSSID();
    
    /**
     * Get current WiFi IP address
     * @return IP address as string
     */
    String getIPAddress();
    
    /**
     * Reset WiFi credentials and restart setup
     */
    void resetWiFi();

private:
    unsigned long _timeout;
    String _ssid;
    String _password;
    
    bool _waitForSerialInput();
    void _printWiFiStatus();
    void _scanNetworks();
};

} // namespace Esp32SerialWifiSetup
