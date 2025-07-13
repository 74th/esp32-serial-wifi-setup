CIで以下のエラーが出たよ。

```
Run arduino/arduino-lint-action@v1
/usr/bin/tar xz --warning=no-unknown-keyword --overwrite -C /home/runner/work/_temp/d406938a-fcba-4c27-8a82-3780eec6fdaf -f /home/runner/work/_temp/aabbcc59-5d13-44d6-97e0-03fe4e3646ac
/opt/hostedtoolcache/arduino-lint/1.3.0/x64/arduino-lint --version
1.3.0 2024-12-19T08:48:16Z
/opt/hostedtoolcache/arduino-lint/1.3.0/x64/arduino-lint --compliance specification --project-type all --library-manager update
Linting library in /home/runner/work/esp32-serial-wifi-setup/esp32-serial-wifi-setup
WARNING: No header file found matching library name (ESP32_Serial_WiFi_Setup.h). Best practices are for primary header
         filename to match library name.
         See: https://arduino.github.io/arduino-cli/latest/library-specification/#libraryproperties-file-format
         (Rule LS008)
WARNING: library.properties name ESP32 Serial WiFi Setup contains spaces. Although supported, best practices is to not
         use spaces.
         See: https://arduino.github.io/arduino-cli/latest/library-specification/#libraryproperties-file-format
         (Rule LP015)
ERROR: Library name ESP32 Serial WiFi Setup not found in the Library Manager index. Library names are not allowed to
       change after being added to the index.
       See: https://github.com/arduino/library-registry/blob/main/FAQ.md#how-can-i-change-a-librarys-name
       (Rule LP018)

Linter results for project: 1 ERRORS, 2 WARNINGS

-------------------

Linting sketch in /home/runner/work/esp32-serial-wifi-setup/esp32-serial-wifi-setup/examples/Basic

Linter results for project: no errors or warnings

-------------------

Linter results for projects: 1 ERRORS, 2 WARNINGS
Error: The process '/opt/hostedtoolcache/arduino-lint/1.3.0/x64/arduino-lint' failed with exit code 1
```

このエラーについて、Copilotに聞いたところ、以下のように返ってきたよ

````
The primary cause of your workflow failure is an error from the Arduino Library Lint step:

- The linter reports:
  > ERROR: Library name ESP32 Serial WiFi Setup not found in the Library Manager index. Library names are not allowed to change after being added to the index.

- There is also a warning:
  > WARNING: library.properties name ESP32 Serial WiFi Setup contains spaces. Although supported, best practice is to not use spaces.

### Solution

#### 1. Update `library.properties` to avoid spaces in the library name

Best practice (and also required for successful library manager registration) is to use a name without spaces and to match the name in the index.
Edit your `library.properties` file:

```ini
name=ESP32SerialWiFiSetup
````

or, if you want to keep the spaces for human readability, use an underscore:

```ini
name=ESP32_SerialWiFiSetup
```

- Make sure that the name matches exactly what you originally submitted to the Arduino Library Manager, as names cannot be changed after acceptance.
- If you've never registered this library before, use a unique, no-space name.

#### 2. Additional Warnings (not critical, but recommended)

- Update all references and documentation to use the new library name.
- If your library is already in the Arduino Library Manager index and you need to change the name for any reason, you must submit a new library under the new name.

#### 3. Deprecated API Usage

Your code uses deprecated ArduinoJson API calls, such as:

```cpp
JsonObject errorObj = resDoc.createNestedObject("error");
```

Update to:

```cpp
JsonObject errorObj = resDoc["error"].to<JsonObject>();
```

Refer to the ArduinoJson migration guide for details.

---

### Summary of Fixes

- Change the `name` in `library.properties` to a no-space version, e.g. `ESP32SerialWiFiSetup`.
- Update deprecated ArduinoJson usages in `src/Esp32SerialWifiSetup.cpp` as shown above.

After making these changes, commit and push. Your workflow should pass the linter.

---

- [library.properties](https://github.com/74th/esp32-serial-wifi-setup/blob/05eeba27c4a60e9c77db0bf6aadef4ca4267c7e5/library.properties)
- [Esp32SerialWifiSetup.cpp](https://github.com/74th/esp32-serial-wifi-setup/blob/05eeba27c4a60e9c77db0bf6aadef4ca4267c7e5/src/Esp32SerialWifiSetup.cpp)

```

これを修正してくれる？
```
