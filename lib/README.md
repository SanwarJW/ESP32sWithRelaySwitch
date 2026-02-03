# Libraries Directory

This directory is designated for project-specific (private) libraries that are part of your ESP32 Relay Controller project.

## Overview

PlatformIO compiles libraries placed in this directory into static libraries and links them into the final executable. This is useful for:
- Custom reusable components
- Third-party libraries not available in PlatformIO registry
- Modified versions of existing libraries
- Project-specific abstractions

## Current Status

**This project currently has no external libraries in this directory.** All functionality is implemented using:
- ESP-IDF framework (core system)
- Standard ESP32 components (WiFi, HTTP server, NVS, GPIO)
- Custom code in `src/` directory

## When to Use This Directory

Consider adding libraries here when you:
- Create reusable components that could be used in other projects
- Need to integrate a third-party library not available via PlatformIO
- Want to isolate complex functionality into separate modules
- Build custom sensor drivers or communication protocols

## Directory Structure for Libraries

If you add a library, follow this structure:

```
lib/
├── README.md                    # This file
├── MyCustomLibrary/
│   ├── src/
│   │   ├── MyCustomLibrary.cpp
│   │   └── MyCustomLibrary.h
│   ├── examples/                # Optional: usage examples
│   │   └── basic_usage.cpp
│   ├── library.json             # Optional: library metadata
│   └── README.md                # Library documentation
└── AnotherLibrary/
    └── ...
```

### Example Library Structure

```
lib/
└── RelayDriver/
    ├── src/
    │   ├── RelayDriver.cpp
    │   └── RelayDriver.h
    ├── library.json
    └── README.md
```

## Example library.json

```json
{
  "name": "RelayDriver",
  "version": "1.0.0",
  "description": "Generic relay control driver",
  "keywords": "relay, gpio, esp32",
  "authors": {
    "name": "Your Name",
    "email": "your.email@example.com"
  },
  "repository": {
    "type": "git",
    "url": "https://github.com/yourusername/relay-driver.git"
  },
  "frameworks": "espidf",
  "platforms": "espressif32"
}
```

## Using Libraries in Your Code

Once a library is added to the `lib/` directory:

1. **PlatformIO automatically detects it** during build
2. **Include the header** in your source files:
   ```c
   #include <MyCustomLibrary.h>
   ```
3. **Use the library functions** as documented

## Library Dependency Finder (LDF)

PlatformIO's LDF automatically:
- Scans your source files for `#include` statements
- Finds matching libraries in `lib/` directory
- Includes them in the build process
- Resolves dependencies between libraries

### LDF Modes

Configure in `platformio.ini`:

```ini
[env:esp32dev]
lib_ldf_mode = deep+        ; Deep+ (recommended): recursive scanning
; Other options: off, chain, deep, chain+
```

## Adding External Libraries

### Method 1: PlatformIO Library Manager (Recommended)

```bash
# Search for a library
pio lib search "library name"

# Install from registry
pio lib install "library-name"

# Install specific version
pio lib install "library-name@1.2.3"
```

Libraries installed this way go to `.pio/libdeps/` (not this directory).

### Method 2: Manual Installation (lib/ directory)

1. Download or clone the library
2. Place in `lib/` directory with proper structure
3. PlatformIO will include it automatically

## Current Project Dependencies

The ESP32 Relay Controller uses these ESP-IDF components (not in lib/):

| Component | Purpose | Documentation |
|-----------|---------|---------------|
| `esp_wifi` | WiFi connectivity | [WiFi API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html) |
| `esp_http_server` | HTTP server | [HTTP Server API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html) |
| `nvs_flash` | Non-volatile storage | [NVS API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html) |
| `driver/gpio` | GPIO control | [GPIO API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html) |
| `esp_log` | Logging system | [Logging API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html) |
| `freertos` | Real-time OS | [FreeRTOS API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html) |

## Example: Creating a Custom Library

### 1. Create Library Structure

```bash
mkdir -p lib/TemperatureSensor/src
cd lib/TemperatureSensor
```

### 2. Create Header (src/TemperatureSensor.h)

```c
#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t temp_sensor_init(uint8_t gpio);
float temp_sensor_read(void);

#endif
```

### 3. Create Implementation (src/TemperatureSensor.c)

```c
#include "TemperatureSensor.h"
#include "driver/gpio.h"

esp_err_t temp_sensor_init(uint8_t gpio) {
    // Implementation here
    return ESP_OK;
}

float temp_sensor_read(void) {
    // Read and return temperature
    return 25.0;
}
```

### 4. Use in Your Project (src/main.c)

```c
#include "TemperatureSensor.h"

void app_main(void) {
    temp_sensor_init(21);
    float temp = temp_sensor_read();
    printf("Temperature: %.1f°C\n", temp);
}
```

## Best Practices

### Do's
- ✅ Keep each library in its own subdirectory
- ✅ Include README.md with usage instructions
- ✅ Use semantic versioning (1.0.0, 1.1.0, etc.)
- ✅ Add examples for complex libraries
- ✅ Document public API functions
- ✅ Use include guards in all headers

### Don'ts
- ❌ Don't put source files directly in `lib/` root
- ❌ Don't mix multiple libraries in one directory
- ❌ Don't include compiled binaries (.a, .o files)
- ❌ Don't depend on specific file paths
- ❌ Don't use global variables without careful consideration

## Troubleshooting

### Library Not Found

**Problem:** Compiler can't find your library

**Solution:**
1. Check directory structure: `lib/LibraryName/src/LibraryName.h`
2. Verify `#include` statement: `#include <LibraryName.h>`
3. Clean and rebuild: `pio run --target clean && pio run`
4. Check LDF mode in `platformio.ini`

### Compilation Errors

**Problem:** Library compiles but causes errors

**Solution:**
1. Check library compatibility with ESP-IDF
2. Verify all dependencies are available
3. Review library documentation for required configuration
4. Check for naming conflicts with ESP-IDF components

### Multiple Definition Errors

**Problem:** Same function defined multiple times

**Solution:**
1. Ensure include guards are present
2. Don't include `.c` files, only `.h` files
3. Check for duplicate library names

## Alternative: lib_deps in platformio.ini

For public libraries, consider using `lib_deps` instead:

```ini
[env:esp32dev]
lib_deps =
    adafruit/Adafruit BusIO@^1.14.1
    bblanchon/ArduinoJson@^6.21.2
```

This is cleaner for public libraries and handles versioning automatically.

## Migration Notes

If you're moving code into libraries:

1. **Identify** reusable components in `src/`
2. **Extract** into library structure in `lib/`
3. **Update** includes in source files
4. **Test** thoroughly
5. **Document** the library API

## Resources

- [PlatformIO Library Management](https://docs.platformio.org/en/latest/librarymanager/)
- [Library Dependency Finder](https://docs.platformio.org/en/latest/librarymanager/ldf.html)
- [Creating a Library](https://docs.platformio.org/en/latest/librarymanager/creating.html)
- [ESP-IDF Component System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html)

## Summary

This `lib/` directory is ready for your custom libraries, but the current ESP32 Relay Controller implementation doesn't require any. All functionality is built using ESP-IDF framework components and custom code in the `src/` directory.

If you expand the project with custom reusable components, this is where they should go!

---

**Quick Start:** To add a library → create `lib/MyLib/src/` → add `.h` and `.c` files → include in your code → rebuild
