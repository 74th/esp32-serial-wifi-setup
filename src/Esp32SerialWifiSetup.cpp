#include "Esp32SerialWifiSetup.h"

namespace Esp32SerialWifiSetup
{

    WiFiSetupManager::WiFiSetupManager()
        : read_line_buf_pos(0)
    {
        memset(read_line_buf, 0, sizeof(read_line_buf));
    }

    void WiFiSetupManager::begin()
    {
        Serial.begin(115200);
        Serial.print("Setup\n");

        // Try to connect with saved credentials
        connectToWiFi();
    }

    void WiFiSetupManager::connectToWiFi()
    {
        wifi_credentials_t c = {0};
        loadWiFiCredentials(c);

        if (c.ssid_len > 0)
        {
            Serial.printf("SSID: %s\n", c.ssid);
            WiFi.begin(c.ssid, c.pass);

            uint8_t count = 10;
            while (WiFi.status() != WL_CONNECTED && count > 0)
            {
                delay(500);
                Serial.print(".");
                count--;
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print("IP:");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("Failed to connect to WiFi");
            }
        }
        else
        {
            Serial.println("no last wifi credentials");
        }
    }

    void WiFiSetupManager::handleSerialCommands()
    {
        handleJsonRpc();
    }

    uint32_t WiFiSetupManager::readLine(char *buf)
    {
        int l = Serial.readBytes(&read_line_buf[read_line_buf_pos], sizeof(read_line_buf) - read_line_buf_pos);
        if (l < 0)
        {
            return 0;
        }

        for (int i = 0; i < l; i++)
        {
            if (read_line_buf[read_line_buf_pos + i] == '\r' && read_line_buf[read_line_buf_pos + i + 1] == '\n')
            {
                uint32_t data_len = read_line_buf_pos + i;
                memcpy(buf, read_line_buf, data_len);
                buf[read_line_buf_pos + i] = '\0';
                memcpy(read_line_buf, &read_line_buf[read_line_buf_pos + i + 2], l - i - 2);
                read_line_buf_pos = l - i - 2;

                return data_len;
            }
        }

        read_line_buf_pos += l;
        return 0;
    }

    void WiFiSetupManager::handleJsonRpc()
    {
        char buf[1024] = {0};
        uint32_t l = readLine(buf);
        if (l == 0)
        {
            return;
        }

        JsonDocument resDoc;
        JsonDocument reqDoc;
        resDoc["jsonrpc"] = "2.0";
        DeserializationError error = deserializeJson(reqDoc, buf);
        if (reqDoc["jsonrpc"] == "2.0")
        {
            resDoc["id"] = reqDoc["id"];
        }
        else
        {
            resDoc["id"] = nullptr;
            JsonObject errorObj = resDoc.createNestedObject("error");
            errorObj["code"] = -32600;
            errorObj["message"] = "Invalid Request";
            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);
            return;
        }

        if (reqDoc["method"] == "set_wifi_creds")
        {
            JsonObject params = reqDoc["params"];
            const char *ssid = params["ssid"];
            const char *pass = params["pass"];
            if (ssid == NULL || pass == NULL)
            {
                JsonObject errorObj = resDoc.createNestedObject("error");
                errorObj["code"] = -32602;
                errorObj["message"] = "Invalid params";
                serializeJson(resDoc, buf, sizeof(buf));
                Serial.println(buf);
                return;
            }

            saveWiFiCredentials(ssid, pass);
            resDoc["result"] = "ok";

            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);

            delay(1000);
            ESP.restart();
        }
        else if (reqDoc["method"] == "get_wifi_creds")
        {
            wifi_credentials_t c = {0};
            loadWiFiCredentials(c);

            JsonObject result = resDoc.createNestedObject("result");
            result["ssid"] = c.ssid;
            result["pass"] = c.pass;

            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);
        }
        else if (reqDoc["method"] == "get_ip")
        {
            JsonObject result = resDoc.createNestedObject("result");

            if (WiFi.status() == WL_CONNECTED)
            {
                result["ip"] = WiFi.localIP().toString();
            }
            else
            {
                result["ip"] = "";
            }

            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);
        }
        else if (reqDoc["method"] == "get_mac_address")
        {
            JsonObject result = resDoc.createNestedObject("result");
            result["mac_address"] = WiFi.macAddress();

            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);
        }
        else
        {
            JsonObject errorObj = resDoc.createNestedObject("error");
            errorObj["code"] = -32601;
            errorObj["message"] = "Method not found";
            serializeJson(resDoc, buf, sizeof(buf));
            Serial.println(buf);
            return;
        }
    }

    void WiFiSetupManager::saveWiFiCredentials(const char *ssid, const char *pass)
    {
        wifi_credentials_t c = {0};
        c.ssid_len = strlen(ssid);
        memcpy(c.ssid, ssid, c.ssid_len);
        c.pass_len = strlen(pass);
        memcpy(c.pass, pass, c.pass_len);

        nvs_handle_t h;
        ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &h));
        ESP_ERROR_CHECK(nvs_set_blob(h, "creds", &c, sizeof(c)));
        nvs_commit(h);
        nvs_close(h);
    }

    void WiFiSetupManager::loadWiFiCredentials(wifi_credentials_t &creds)
    {
        nvs_handle_t h;
        esp_err_t err = nvs_open("wifi", NVS_READONLY, &h);
        if (err == ESP_OK)
        {
            size_t sz = sizeof(wifi_credentials_t);
            nvs_get_blob(h, "creds", &creds, &sz);
            nvs_close(h);
        }
    }

    bool WiFiSetupManager::isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    String WiFiSetupManager::getIPAddress()
    {
        return WiFi.localIP().toString();
    }

    String WiFiSetupManager::getMacAddress()
    {
        return WiFi.macAddress();
    }

} // namespace Esp32SerialWifiSetup
