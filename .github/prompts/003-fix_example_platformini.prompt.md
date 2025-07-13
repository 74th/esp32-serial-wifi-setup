一応、examples/Basicにある物も同様に動くようにしてくれる？

IRRemoteESP8266にあるexamples/IRrecvDumpV2/platformio.iniは以下のようになっていたので参考にして。

```
[platformio]
src_dir = .

[env]
; Default platform
platform = espressif8266
; Default board
board = nodemcuv2
framework = arduino
lib_extra_dirs = ../../
lib_ldf_mode = deep+
lib_ignore = examples
monitor_speed = 115200
build_flags = ; -D_IR_LOCALE_=en-AU
```
