#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <esp_system.h>
#include "nvs_flash.h"

namespace Esp32SerialWifiSetup
{
    typedef struct
    {
        char ssid_len;
        char ssid[32];
        char pass_len;
        char pass[64];
    } wifi_credentials_t;

    class WiFiSetupManager
    {
    public:
        WiFiSetupManager();

        /**
         * Initialize the WiFi setup manager and try to connect with saved credentials
         */
        void begin();

        /**
         * Handle JSON-RPC commands from serial port
         * Call this function in your main loop
         */
        void handleSerialCommands();

        /**
         * Check if WiFi is connected
         * @return true if connected, false otherwise
         */
        bool isConnected();

        /**
         * Get current WiFi IP address
         * @return IP address as string
         */
        String getIPAddress();

        /**
         * Get current WiFi MAC address
         * @return MAC address as string
         */
        String getMacAddress();

    private:
        char read_line_buf[1024];
        int read_line_buf_pos;

        uint32_t readLine(char *buf);
        void handleJsonRpc();
        void connectToWiFi();
        void saveWiFiCredentials(const char *ssid, const char *pass);
        void loadWiFiCredentials(wifi_credentials_t &creds);
    };

} // namespace Esp32SerialWifiSetup
