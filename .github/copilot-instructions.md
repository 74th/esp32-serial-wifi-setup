# ESP32 Serial WiFi Setup Library - Copilot Instructions

## Project Overview

This is an ESP32 Arduino library that enables WiFi configuration via serial communication using JSON-RPC 2.0 protocol. The library persists WiFi credentials in ESP32's NVS (Non-Volatile Storage) and automatically reconnects on boot.

## Architecture & Key Components

### Core Library Structure
- **`src/Esp32SerialWifiSetup.h/.cpp`**: Main library implementation with `WiFiSetupManager` class
- **`examples/Basic/`**: Example Arduino sketch demonstrating library usage
- **Dual build system**: Root `platformio.ini` builds examples, `examples/Basic/platformio.ini` for standalone builds

### JSON-RPC Protocol Implementation
The library implements 4 JSON-RPC methods:
- `set_wifi_creds`: Store SSID/password, triggers ESP32 restart
- `get_wifi_creds`: Retrieve stored credentials
- `get_ip`: Get current IP address
- `get_mac_address`: Get device MAC address

**Critical Pattern**: All JSON-RPC responses follow strict format:
```json
{"jsonrpc": "2.0", "id": <request_id>, "result": {...}}
{"jsonrpc": "2.0", "id": <request_id>, "error": {"code": <code>, "message": "<msg>"}}
```

### Data Flow & State Management
1. **Initialization**: `begin()` loads saved credentials from NVS and attempts auto-connection
2. **Runtime**: `handleSerialCommands()` processes incoming JSON-RPC via serial buffer
3. **Persistence**: WiFi credentials stored in NVS namespace "wifi" with key "creds"

## Development Workflows

### Building & Testing
- **Root build**: `~/.platformio/penv/bin/pio run -e esp32dev` (builds Basic example)
- **Example build**: `cd examples/Basic && ~/.platformio/penv/bin/pio run`
- **Serial testing**: Send JSON-RPC commands via serial monitor at 115200 baud

### Adding New JSON-RPC Methods
Extend `handleJsonRpc()` method following the existing pattern:
```cpp
else if (reqDoc["method"] == "your_method") {
    JsonObject result = resDoc.createNestedObject("result");
    // Implementation
    serializeJson(resDoc, buf, sizeof(buf));
    Serial.println(buf);
}
```

## Project-Specific Conventions

### PlatformIO Configuration Pattern
- **Root**: `src_dir = examples/Basic` with `lib_extra_dirs = .` for library reference
- **Examples**: `lib_extra_dirs = ../../` to reference root library source
- **ESP32 boards**: Default esp32dev, with esp32-c3 and esp32-s3 variants defined

### Memory Management
- Fixed 1024-byte serial buffer (`read_line_buf`) for JSON-RPC parsing
- NVS credentials struct with explicit length fields for SSID (32 bytes) and password (64 bytes)
- ArduinoJson v7 with deprecated `createNestedObject()` pattern (known warnings)

### Error Handling Patterns
- JSON-RPC errors use standard codes: -32600 (Invalid Request), -32601 (Method not found), -32602 (Invalid params)
- ESP32 restart triggered after successful WiFi credential setting
- WiFi connection timeout: 10 attempts with 500ms delays

## Integration Points

### Dependencies
- **ArduinoJson ^7**: JSON parsing/serialization (note: v7 deprecation warnings expected)
- **ESP32 Arduino Core**: WiFi, NVS, and system functions
- **ESP32 NVS**: Credential persistence in "wifi" namespace

### Serial Communication
- **Baud rate**: 115200 (hardcoded in `begin()`)
- **Line termination**: CRLF (`\r\n`) required for JSON-RPC commands
- **Buffer management**: Circular buffer with line-based parsing in `readLine()`

## Critical Implementation Details

### WiFi Connection Logic
Auto-connection attempts saved credentials on `begin()`, with connection status available via `isConnected()`. Failed connections don't prevent JSON-RPC operation.

### Namespace Usage
All library code wrapped in `Esp32SerialWifiSetup` namespace to avoid conflicts. Examples use fully qualified names: `Esp32SerialWifiSetup::WiFiSetupManager`.

### Build System Peculiarities
- Root build compiles from `examples/Basic/` source directory
- Library lookup via `lib_extra_dirs` prevents need for separate library installation
- `lib_ignore = examples` prevents circular dependencies
