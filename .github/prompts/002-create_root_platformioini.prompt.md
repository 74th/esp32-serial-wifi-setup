---
mode: agent
---

IRremoteESP8266というESP32向けArduinoライブラリの、リポジトリのルートにあるlatformIO設定ファイルが以下のようになっていたよ。
これを参考にして、ルートディレクトリのplatformio.iniファイルを作成し、examples/Basicのプロジェクトをルートディレクトリでビルドできるようにして。
もとのツールはESP8266向けのものだけれど、今回はESP32向けの設定にしてね

```ini
[platformio]
# Default to building IRrecvDumpV2 if not in a specific example directory.
src_dir = examples/IRrecvDumpV2

[env]
lib_extra_dirs = .
lib_ldf_mode = deep+
lib_ignore = examples
framework = arduino
platform = espressif8266
build_flags = ; -D_IR_LOCALE_=en-AU
monitor_speed = 115200

[env:nodemcuv2]
board = nodemcuv2

[env:d1_mini]
board = d1_mini

[env:esp32dev]
platform = espressif32
board = esp32dev
```
