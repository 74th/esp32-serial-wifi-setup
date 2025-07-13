# ESP32 Serial WiFi Setup

ESP32でJSON-RPC プロトコルを使ってシリアル通信経由でWiFi設定を行うためのライブラリです。

## 特徴

- JSON-RPC 2.0プロトコルによるWiFi設定
- NVS（Non-Volatile Storage）を使用した設定の永続化
- シンプルで使いやすいAPI
- Arduino IDEとPlatformIOの両方に対応

## インストール

### Arduino IDE

1. このリポジトリをZIPでダウンロード
2. Arduino IDE で「スケッチ」→「ライブラリをインクルード」→「.ZIP形式のライブラリをインストール」
3. ダウンロードしたZIPファイルを選択

### PlatformIO

`platformio.ini` に以下を追加：

```ini
lib_deps =
    https://github.com/74th/esp32-serial-wifi-setup.git
    bblanchon/ArduinoJson@^7
```

## 使用方法

### 基本的な使い方

```cpp
#include <Esp32SerialWifiSetup.h>

Esp32SerialWifiSetup::WiFiSetupManager wifiSetup;

void setup() {
    wifiSetup.begin();
}

void loop() {
    wifiSetup.handleSerialCommands();
}
```

### JSON-RPC コマンド

シリアルポートに以下のJSON-RPCコマンドを送信してWiFiを設定します：

#### WiFi認証情報の設定

```json
{"jsonrpc": "2.0", "method": "set_wifi_creds", "params": {"ssid": "your_wifi_ssid", "pass": "your_password"}, "id": 1}
```

応答:

```json
{"jsonrpc": "2.0", "id": 1, "result": "ok"}
```

**注意**: 設定後、ESP32は自動的に再起動します。

#### WiFi認証情報の取得

```json
{"jsonrpc": "2.0", "method": "get_wifi_creds", "id": 1}
```

応答:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"ssid": "your_wifi_ssid", "pass": "your_password"}}
```

#### IPアドレスの取得

```json
{"jsonrpc": "2.0", "method": "get_ip", "id": 1}
```

応答:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"ip": "192.168.1.109"}}
```

#### MACアドレスの取得

```json
{"jsonrpc": "2.0", "method": "get_mac_address", "id": 1}
```

応答:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"mac_address": "A0:76:4E:B3:67:DC"}}
```

## API リファレンス

### `WiFiSetupManager`

#### `void begin()`

ライブラリを初期化し、保存されたWiFi認証情報があれば自動的に接続を試行します。

#### `void handleSerialCommands()`

シリアルポートからのJSON-RPCコマンドを処理します。メインループで継続的に呼び出してください。

#### `bool isConnected()`

WiFi接続状態を返します。

#### `String getIPAddress()`

現在のIPアドレスを文字列で返します。

#### `String getMacAddress()`

MACアドレスを文字列で返します。

## 依存関係

- ArduinoJson (バージョン 7.x)
- ESP32 Arduino Core

## ライセンス

MIT License

## 貢献

バグ報告や機能要求は、GitHubのIssuesでお知らせください。
