---
mode: agent
---

ESP32マイコンを使うのだけれど、Arduino IDEと、PlatformIOのArduino開発で使えるライブラリを作りたい。
そのプロジェクトのテンプレートを作って。

以下は、作成方法についてまとめた記事だよ。

---

以下は「Arduino IDE」「PlatformIO (Framework = Arduino)」のどちらでもそのまま取り込める ESP32 用ライブラリを用意するための実践的な手順です。最終的に **1 つの GitHub リポジトリ** で両方のエコシステムに対応できます。

---

## 1. 最低限のディレクトリ構成

```
YourLib/
├─ src/                // .h/.cpp だけを置く
│   ├─ YourLib.h
│   └─ YourLib.cpp
├─ examples/
│   └─ Basic/Basic.ino // Arduino IDE でも PIO でも開ける
├─ library.properties  // Arduino Library Manifest
├─ library.json        // PlatformIO Manifest
├─ keywords.txt        // IDE ハイライト (任意)
├─ README.md           // 使い方
└─ LICENSE             // SPDX 互換ライセンス
```

- **src/** 直下以外のソースをビルドに含めないこと（PlatformIO は src/ を再帰的に走査します）。
- **examples/** は動くサンプルを必ず配置。Arduino Library Manager 登録時の必須要件です。

---

## 2. `library.properties`（Arduino IDE 用）

```properties
name=YourLib
version=1.0.0
author=Atsushi Morimoto <foo@example.com>
maintainer=Atsushi Morimoto <foo@example.com>
license=MIT
sentence=Light-weight helper library for ESP32 projects.
paragraph=Provides ... (80 文字以内)
category=Communication
url=https://github.com/yourname/YourLib
architectures=esp32
```

- **必須フィールド**: name / version / author / maintainer / sentence / paragraph / category / url / architectures。Arduino 1.5+ 仕様です。([docs.arduino.cc][1])
- `architectures=*` にすると他 MCU でもインストール出来ますが、ESP32 固有 API を含むなら `esp32` に絞っておくとユーザが混乱しません。
- 追加チェック用に **arduino-lint** を GitHub Actions で回すと安心。([arduino.github.io][2])

---

## 3. `library.json`（PlatformIO 用）

```json
{
  "name": "YourLib",
  "version": "1.0.0",
  "description": "Light-weight helper library for ESP32 projects.",
  "keywords": ["esp32", "arduino"],
  "license": "MIT",
  "homepage": "https://github.com/yourname/YourLib",
  "repository": {
    "type": "git",
    "url": "https://github.com/yourname/YourLib.git"
  },
  "frameworks": ["arduino"],
  "platforms": ["espressif32"],
  "authors": [
    {
      "name": "Atsushi Morimoto",
      "email": "foo@example.com",
      "maintainer": true
    }
  ],
  "dependencies": [{ "name": "ArduinoJson", "version": "^7" }]
}
```

- PlatformIO は **`library.properties` だけでも依存解決付きでビルドできます** が、レジストリ公開 (`pio pkg publish`) には `library.json` が必須です。([docs.platformio.org][3], [docs.platformio.org][4])
- `frameworks` と `platforms` で **Arduino × ESP32** に限定。
- 依存ライブラリがある場合は `dependencies` を明示すると自動インストールされます。

---

## 4. GitHub リポジトリとリリース運用

| 作業         | ポイント                                                                                                                                                                                                |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **タグ付け** | `v1.0.0` 形式の Git タグを push。Arduino Library Manager は SemVer タグを検出して自動同期します。                                                                                                       |
| **CI**       | `pio run` で最低 1 ボード（例 `esp32dev`）のビルドをテスト。失敗したら公開しない。                                                                                                                      |
| **リリース** | ① GitHub Release を発行<br>② Arduino Library Manager に PR（`arduino/library-registry` repo）<br>③ `pio pkg publish` で PlatformIO Registry へアップロード。必要に応じ `pio pkg unpublish` で取り下げ。 |

---

## 5. ソースコード上の注意

```cpp
#pragma once
#include <Arduino.h>   // Arduino core

namespace YourLib {
class Foo {
public:
  void begin() {
#if defined(ESP32)
    // ESP32専用API
#endif
  }
};
}  // namespace YourLib
```

- `#if defined(ARDUINO_ARCH_ESP32)` または `CONFIG_IDF_TARGET_ESP32*` 判定でポータビリティを確保。
- `.cpp` 側で `#include "YourLib.h"` のみを使い、外部ヘッダの include パス競合を避ける。

---

## 6. 動作確認フロー

1. **Arduino IDE**

   - 1.8 系なら「スケッチ→ライブラリをインクルード→.zip 形式で追加」
   - 2.x 系なら「ライブラリマネージャ」で検索。

2. **PlatformIO**

   - `platformio.ini` に

     ```ini
     lib_deps = yourname/YourLib @ ^1.0.0
     ```

   - `pio run --target upload` でビルド＆書き込み。

---

### まとめ

- **両 Manifest を同梱**することで IDE 依存のないライブラリを実現。
- **SemVer タグと CI** を回すことで Arduino Library Manager・PlatformIO Registry の両方へ自動的に反映。
- この構成のまま **ZIP で配布**してもどちらのツールチェーンでも “そのまま使える” ので、ESP32 プロジェクトの再利用性が高まります。

[1]: https://docs.arduino.cc/arduino-cli/library-specification?utm_source=chatgpt.com "Library specification - Arduino Documentation"
[2]: https://arduino.github.io/arduino-lint/dev/rules/library/?utm_source=chatgpt.com "Library - Arduino Lint"
[3]: https://docs.platformio.org/en/latest/manifests/library-json/index.html?utm_source=chatgpt.com "library.json — PlatformIO latest documentation"
[4]: https://docs.platformio.org/en/latest/librarymanager/creating.html?utm_source=chatgpt.com "Creating Library — PlatformIO latest documentation"
