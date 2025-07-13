# ESP32 Serial WiFi Setup

*[日本語版 (Japanese Version)](README_ja.md)*

A library for ESP32 that enables WiFi configuration via serial communication using JSON-RPC protocol.

## Features

- WiFi configuration using JSON-RPC 2.0 protocol
- Persistent settings using NVS (Non-Volatile Storage)
- Simple and easy-to-use API
- Compatible with both Arduino IDE and PlatformIO

## Installation

### Arduino IDE

1. Install ArduinoJson library
   - In Arduino IDE, go to "Tools" → "Manage Libraries"
   - Search for "ArduinoJson"
   - Install "ArduinoJson" by Benoit Blanchon (version 7.x)
2. Download this repository as ZIP
3. In Arduino IDE, go to "Sketch" → "Include Library" → "Add .ZIP Library"
4. Select the downloaded ZIP file

Dependencies (ArduinoJson) will be installed automatically.

### PlatformIO

Add the following to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/74th/esp32-serial-wifi-setup.git
```

## Usage

### Basic Usage

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

### Web Interface

You can configure WiFi settings from your browser using WebUSB Serial API:

**🌐 Visit [https://esp32-serial-wifi-setup.74th.tech](https://esp32-serial-wifi-setup.74th.tech)**

Web interface features:
- Direct connection to ESP32 from browser (using WebUSB Serial API)
- WiFi credentials configuration (SSID/Password)
- Real-time serial communication monitoring
- IP address and MAC address retrieval
- No additional software installation required

**Requirements:**
- WebUSB-compatible browser (Chrome/Edge, etc.)
- HTTPS connection (required)

### JSON-RPC Commands

Configure WiFi by sending the following JSON-RPC commands to the serial port:

#### Set WiFi Credentials

```json
{"jsonrpc": "2.0", "method": "set_wifi_creds", "params": {"ssid": "your_wifi_ssid", "pass": "your_password"}, "id": 1}
```

Response:

```json
{"jsonrpc": "2.0", "id": 1, "result": "ok"}
```

**Note**: The ESP32 will automatically restart after setting credentials.

#### Get WiFi Credentials

```json
{"jsonrpc": "2.0", "method": "get_wifi_creds", "id": 1}
```

Response:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"ssid": "your_wifi_ssid", "pass": "your_password"}}
```

#### Get IP Address

```json
{"jsonrpc": "2.0", "method": "get_ip", "id": 1}
```

Response:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"ip": "192.168.1.109"}}
```

#### Get MAC Address

```json
{"jsonrpc": "2.0", "method": "get_mac_address", "id": 1}
```

Response:

```json
{"jsonrpc": "2.0", "id": 1, "result": {"mac_address": "A0:76:4E:B3:67:DC"}}
```

## Limitations

- **Line Endings**: Only CRLF (`\r\n`) line endings are supported when sending JSON-RPC commands
- **Baud Rate**: Serial communication baud rate is fixed at 115200

## API Reference

### `WiFiSetupManager`

#### `void begin()`

Initializes the library and automatically attempts to connect using saved WiFi credentials if available.

#### `void handleSerialCommands()`

Processes JSON-RPC commands from the serial port. Call this continuously in your main loop.

#### `bool isConnected()`

Returns the WiFi connection status.

#### `String getIPAddress()`

Returns the current IP address as a string.

#### `String getMacAddress()`

Returns the MAC address as a string.

## Dependencies

- ArduinoJson (version 7.x)
- ESP32 Arduino Core

## License

MIT License

## Contributing

Please report bugs and feature requests through GitHub Issues.
