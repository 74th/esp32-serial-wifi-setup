#include <Esp32SerialWifiSetup.h>

// This is a dummy main file for library compilation check
void setup()
{
#ifdef LIB_CHECK_ONLY
    // Library compilation check - no actual functionality needed
    Esp32SerialWifiSetup::WiFiSetupManager manager;
    manager.begin();
#endif
}

void loop()
{
#ifdef LIB_CHECK_ONLY
    // Empty loop for compilation check
#endif
}
