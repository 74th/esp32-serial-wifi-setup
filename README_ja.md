# ESP32 Serial WiFi Setup

ESP32でJSON-RPC プロトコルを使ってシリアル通信経由でWiFi設定を行うためのライブラリです。

## 特徴

- JSON-RPC 2.0プロトコルによるWiFi設定
- NVS（Non-Volatile Storage）を使用した設定の永続化
- シンプルで使いやすいAPI
- Arduino IDEとPlatformIOの両方に対応

## インストール

### Arduino IDE

1. ArduinoJsonライブラリをインストール
   - Arduino IDE で「ツール」→「ライブラリを管理」
   - 検索欄に「ArduinoJson」と入力
   - Benoit Blanchon作の「ArduinoJson」をインストール（バージョン7.x）
2. このリポジトリをZIPでダウンロード
3. Arduino IDE で「スケッチ」→「ライブラリをインクルード」→「.ZIP形式のライブラリをインストール」
4. ダウンロードしたZIPファイルを選択

依存関係（ArduinoJson）は自動的にインストールされます。

### PlatformIO

`platformio.ini` に以下を追加：

```ini
lib_deps =
    https://github.com/74th/esp32-serial-wifi-setup.git
```

## 使用方法

### 基本的な使い方

```cpp
#include <Esp32SerialWiFiSetup.h>

Esp32SerialWifiSetup::WiFiSetupManager wifiSetup;

void setup() {
    wifiSetup.begin();
}

void loop() {
    wifiSetup.handleSerialCommands();
}
```

### Web インターフェース

ブラウザからWebUSB Serial APIを使用してWiFi設定を行うことができます：

**🌐 [https://esp32-serial-wifi-setup.74th.tech](https://esp32-serial-wifi-setup.74th.tech) にアクセス**

Webインターフェースの特徴：
- ブラウザから直接ESP32に接続（WebUSB Serial API使用）
- WiFi認証情報（SSID/パスワード）の設定
- リアルタイムでのシリアル通信モニタリング
- IPアドレスとMACアドレスの取得
- 追加ソフトウェアのインストール不要

**必要要件:**
- Chrome/Edge等のWebUSB対応ブラウザ
- HTTPS接続（必須）

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

## 制限事項

- **改行コード**: JSON-RPCコマンドの送信時は、CRLF（`\r\n`）の改行コードのみ対応しています
- **ボーレート**: シリアル通信のボーレートは115200固定です

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
