# ESP32 Serial WiFi Setup

ESP32でシリアル通信を通じて簡単にWiFi設定を行うためのライブラリです。

## 概要

このライブラリを使用すると、ESP32プロジェクトでWiFi認証情報をハードコードすることなく、シリアル通信を通じて動的にWiFi設定を行うことができます。

## 特徴

- シリアル通信による簡単なWiFi設定
- 利用可能なネットワークの自動スキャン
- 接続状態の監視
- Arduino IDEとPlatformIOの両方に対応
- ESP32専用に最適化

## インストール

### Arduino IDE

1. Arduino IDE のライブラリマネージャから「ESP32 Serial WiFi Setup」を検索してインストール
2. または、このリポジトリをZIPでダウンロードし、「スケッチ」→「ライブラリをインクルード」→「.ZIP形式のライブラリをインストール」

### PlatformIO

`platformio.ini` に以下を追加:

```ini
lib_deps = 74th/ESP32 Serial WiFi Setup @ ^1.0.0
```

## 使用方法

### 基本的な使用例

```cpp
#include <Esp32SerialWifiSetup.h>

Esp32SerialWifiSetup::WiFiSetupManager wifiSetup;

void setup() {
    // WiFi設定マネージャを初期化
    wifiSetup.begin();

    // WiFi設定プロセスを開始
    if (wifiSetup.setupWiFi()) {
        Serial.println("WiFi接続成功！");
        Serial.print("SSID: ");
        Serial.println(wifiSetup.getSSID());
        Serial.print("IP Address: ");
        Serial.println(wifiSetup.getIPAddress());
    } else {
        Serial.println("WiFi接続に失敗しました");
    }
}

void loop() {
    // 接続状態を確認
    if (wifiSetup.isConnected()) {
        // WiFi接続中の処理
    }
}
```

### 設定手順

1. ESP32にスケッチをアップロード
2. シリアルモニタを開く（ボーレート: 115200）
3. 利用可能なWiFiネットワークが表示されます
4. 接続したいネットワークのSSIDを入力
5. パスワードを入力
6. 自動的に接続が試行されます

## API リファレンス

### `WiFiSetupManager`

#### `begin(unsigned long timeout_ms = 30000)`

WiFi設定マネージャを初期化します。

- `timeout_ms`: ユーザー入力の待機時間（デフォルト: 30秒）

#### `setupWiFi()`

WiFi設定プロセスを開始します。

- 戻り値: 接続成功時は `true`、失敗時は `false`

#### `isConnected()`

WiFi接続状態を確認します。

- 戻り値: 接続中は `true`、未接続は `false`

#### `getSSID()`

現在接続中のWiFiネットワークのSSIDを取得します。

- 戻り値: SSID文字列

#### `getIPAddress()`

現在のIPアドレスを取得します。

- 戻り値: IPアドレス文字列

#### `resetWiFi()`

WiFi認証情報をリセットし、接続を切断します。

## 依存関係

- ArduinoJson (v7.x)
- ESP32 Arduino Core

## 対応プラットフォーム

- ESP32 (Arduino Framework)

## ライセンス

MIT License

## 貢献

プルリクエストや課題報告を歓迎します。

## 作者

Atsushi Morimoto (74th) - 74th@nnyn.dev

## リリース履歴

- v1.0.0: 初回リリース
