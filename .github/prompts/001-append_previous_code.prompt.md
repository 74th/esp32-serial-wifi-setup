---
mode: agent
---

今あるコードは、Agentに適当に作らせたテンプレートコードになっていて、機能はしないよ。README.mdにある内容は参考にしないで。

Serial経由でWiFiを設定するために、以下の様にSerialにJSON文を送ることで、JSON-RPCのプロトコルで、WiFiの設定をマイコン中に保存させて、利用できるようにするよ。

```
// WiFiの設定
{"jsonrpc": "2.0", "method": "set_wifi_creds", "params": {"ssid": "hoge", "pass": "foo"}, "id":1}
// 応答
{"jsonrpc":"2.0","id":1,"result":"ok"}

// WiFiの設定を取得
{"jsonrpc": "2.0", "method": "get_wifi_creds", "id":1}
// 応答
{"jsonrpc":"2.0","id":1,"result":{"ssid":"hoge","pass":"foo"}}

// IPアドレスの取得
{"jsonrpc": "2.0", "method": "get_ip", "id":1}
// 応答
{"jsonrpc":"2.0","id":1,"result":{"ip":"192.168.1.109"}}

// Macアドレスを取得
{"jsonrpc": "2.0", "method": "get_mac_address", "id":1}
// 応答
{"jsonrpc":"2.0","id":1,"result":{"mac_address":"A0:76:4E:B3:67:DC"}}
```

Serial経由でWiFiを設定するためのコードを、以下のように作ったよ。
これを、ライブラリとして使えるように、以下のコードを参考に、 src/ 以下にライブラリを実装してね。
加えて、examples/Basic/ 以下に、利用するサンプルコードを書いてね。

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <esp_system.h>
#include "nvs_flash.h"

typedef struct
{
  char ssid_len;
  char ssid[32];
  char pass_len;
  char pass[64];
} wifi_credentials_t;

char read_line_buf[1024] = {0};
int read_line_buf_pos = 0;

void setup()
{
  Serial.begin(115200);
  Serial.print("Setup\n");

  nvs_handle_t h;
  nvs_open("wifi", NVS_READONLY, &h);
  size_t sz = sizeof(wifi_credentials_t);
  wifi_credentials_t c = {0};
  nvs_get_blob(h, "creds", &c, &sz);
  nvs_close(h);
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

uint32_t read_line(char *buf)
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

void handle_json_rpc()
{
  char buf[1024] = {0};
  uint32_t l = read_line(buf);
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
    resDoc["id"] = NULL;
    JsonObject errorObj = resDoc.createNestedObject("error");
    errorObj["code"] = -32600;
    errorObj["message"] = "Invalid Request";
    serializeJson(resDoc, buf, sizeof(buf));
    Serial.println(buf);
    return;
  }

  if (reqDoc["method"] == "set_wifi_creds")
  {
    wifi_credentials_t c = {0};

    JsonObject params = reqDoc["params"];
    const char *ssid = params["ssid"];
    const char *pass = params["pass"];
    if (ssid == NULL || pass == NULL)
    {
      return;
    }
    c.ssid_len = strlen(ssid);
    memcpy(c.ssid, ssid, c.ssid_len);
    c.pass_len = strlen(pass);
    memcpy(c.pass, pass, c.pass_len);

    nvs_handle_t h;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READWRITE, &h));
    ESP_ERROR_CHECK(nvs_set_blob(h, "creds", &c, sizeof(c)));
    nvs_commit(h);

    resDoc["result"] = "ok";

    serializeJson(resDoc, buf, sizeof(buf));
    Serial.println(buf);

    delay(1000);

    ESP.restart();
  }
  else if (reqDoc["method"] == "get_wifi_creds")
  {
    nvs_handle_t h;
    ESP_ERROR_CHECK(nvs_open("wifi", NVS_READONLY, &h));
    size_t sz = sizeof(wifi_credentials_t);
    wifi_credentials_t c = {0};
    ESP_ERROR_CHECK(nvs_get_blob(h, "creds", &c, &sz));
    nvs_close(h);

    JsonObject result = resDoc.createNestedObject("result");
    result["ssid"] = c.ssid;
    result["pass"] = c.pass;

    serializeJson(resDoc, buf, sizeof(buf));
    Serial.println(buf);
  }
  else if (reqDoc["method"] == "get_ip")
  {
    resDoc["result"] = "ok";

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
    resDoc["result"] = "ok";

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

void loop()
{
  handle_json_rpc();
}
```
