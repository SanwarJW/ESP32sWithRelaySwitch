# Include Directory

This directory contains all header files (.h) for the ESP32 Relay Controller project.

## Overview

Header files define interfaces, data structures, and configuration parameters used throughout the project. They allow separation of interface (what) from implementation (how).

## Files

### Configuration Files

#### `config.h`
**Main configuration file** - Contains all tunable parameters for the system.

**What to configure:**
- WiFi credentials (SSID, password)
- Network settings (static IP, gateway, subnet)
- Relay GPIO pin assignments
- Relay names for display
- Relay behavior (active LOW/HIGH, default state)
- HTTP server parameters (connections, timeouts)
- Performance tuning options

**Example:**
```c
#define WIFI_SSID           "YourNetwork"
#define WIFI_PASSWORD       "YourPassword"
#define STATIC_IP           "192.168.1.100"
#define RELAY_1_GPIO        16
#define RELAY_1_NAME        "Living Room Light"
```

### Service Interfaces

#### `wifi_service.h`
Defines the WiFi management service interface.

**Key functions:**
- `wifi_service_init()` - Initialize and connect to WiFi
- `wifi_is_connected()` - Check connection status
- `wifi_get_ip_address()` - Get current IP address
- `wifi_reconnect()` - Force reconnection attempt

**Features:**
- Automatic connection with retry logic
- Static IP or DHCP support
- Connection monitoring
- Event-driven status updates

#### `relay_service.h`
Defines the relay control service interface.

**Key functions:**
- `relay_service_init()` - Initialize GPIO pins and load saved states
- `relay_toggle(id)` - Toggle relay on/off
- `relay_set_state(id, state)` - Set specific state (on/off)
- `relay_get_state(id)` - Get current relay state
- `relay_get_info(id)` - Get relay information (name, GPIO, state)
- `relay_all_on()` - Turn all relays on
- `relay_all_off()` - Turn all relays off

**Features:**
- State persistence to NVS (Non-Volatile Storage)
- Configurable active LOW/HIGH logic
- Safe defaults (all relays OFF on boot)
- Built-in LED feedback

#### `http_controller.h`
Defines the HTTP server and REST API interface.

**Key functions:**
- `http_controller_init()` - Start HTTP server
- `http_controller_stop()` - Stop HTTP server
- `http_controller_get_handle()` - Get server handle (for health checks)

**Endpoints provided:**
- Web UI: `GET /`
- Toggle: `GET /relay/{id}/toggle`
- Control: `GET /relay/{id}/on`, `GET /relay/{id}/off`
- Status: `GET /relay/{id}/status` (JSON)
- Bulk: `GET /relay/all/on`, `GET /relay/all/off`

**Features:**
- RESTful API design
- JSON responses
- Keep-alive connections
- Error handling and validation

#### `ui_templates.h`
Contains HTML templates for the web interface.

**What it includes:**
- Complete responsive HTML/CSS/JavaScript
- Toggle button UI for each relay
- Real-time state updates
- Mobile-friendly design
- Embedded directly in firmware (no SD card needed)

**Features:**
- Clean, modern interface
- Color-coded relay states (green=ON, gray=OFF)
- One-click toggle controls
- Displays relay names from configuration
- Built with inline CSS and JavaScript

## Usage in Your Code

To use these services in your source files:

```c
// Include necessary headers
#include "config.h"
#include "wifi_service.h"
#include "relay_service.h"
#include "http_controller.h"

void app_main(void) {
    // Initialize services
    wifi_service_init();
    relay_service_init();
    http_controller_init();
    
    // Control relays
    relay_set_state(0, RELAY_ON);  // Turn on relay 0
    relay_toggle(1);                // Toggle relay 1
    
    // Check status
    if (wifi_is_connected()) {
        ESP_LOGI("APP", "IP: %s", wifi_get_ip_address());
    }
}
```

## File Organization

```
include/
├── README.md              # This file
├── config.h              # Configuration parameters
├── wifi_service.h        # WiFi management interface
├── relay_service.h       # Relay control interface
├── http_controller.h     # HTTP server interface
└── ui_templates.h        # Web UI HTML templates
```

## Header File Best Practices

### Include Guards
All header files use include guards to prevent multiple inclusion:

```c
#ifndef CONFIG_H
#define CONFIG_H

// ... declarations ...

#endif // CONFIG_H
```

### Documentation
Each header file includes:
- File description (purpose and usage)
- Function documentation (parameters, returns, behavior)
- Data structure documentation
- Example usage when appropriate

### Dependencies
Header files only include what they need:
- ESP-IDF system headers (`esp_err.h`, `esp_http_server.h`, etc.)
- Standard C headers (`stdbool.h`, `stdint.h`)
- Other project headers only when necessary

## Modifying Headers

### Changing Configuration

To customize your deployment:

1. **Open** `config.h`
2. **Find** the setting you want to change
3. **Modify** the value
4. **Rebuild** the project (`pio run`)
5. **Upload** to ESP32 (`pio run --target upload`)

### Adding New Features

If you add new functions to source files:

1. **Declare** the function in the appropriate header
2. **Document** with comments explaining parameters and return values
3. **Use** standard ESP-IDF types (`esp_err_t` for return codes)
4. **Include** any new dependencies

Example:

```c
/**
 * @brief Get relay uptime in seconds
 * 
 * @param relay_id Relay index (0-3)
 * @return Uptime in seconds since last state change, or -1 on error
 */
int relay_get_uptime(uint8_t relay_id);
```

## Common Header File Patterns

### ESP-IDF Error Handling
```c
esp_err_t my_function(void) {
    // Return ESP_OK on success
    // Return error codes like ESP_FAIL, ESP_ERR_INVALID_ARG, etc.
    return ESP_OK;
}
```

### Boolean Returns
```c
bool is_something_enabled(void) {
    return true;  // or false
}
```

### Data Structures
```c
typedef struct {
    uint8_t id;
    char name[32];
    bool enabled;
} my_config_t;
```

## Integration with ESP-IDF

These headers work seamlessly with ESP-IDF:

- Use ESP-IDF logging macros (`ESP_LOGI`, `ESP_LOGE`, etc.)
- Return ESP error codes (`ESP_OK`, `ESP_FAIL`)
- Follow ESP-IDF naming conventions
- Compatible with FreeRTOS and ESP-IDF services

## Troubleshooting

### Compilation Errors

**Missing header:**
```
fatal error: some_file.h: No such file or directory
```
**Solution:** Check the `#include` path and ensure file exists in `include/` directory.

**Redefinition errors:**
```
error: redefinition of 'function_name'
```
**Solution:** Ensure include guards are present and unique.

### Configuration Issues

**Wrong WiFi credentials:**
- Update `WIFI_SSID` and `WIFI_PASSWORD` in `config.h`
- Rebuild and upload

**IP address conflicts:**
- Change `STATIC_IP` in `config.h`
- Ensure it's not used by another device

**Relays not working:**
- Verify GPIO pins in `config.h` match your wiring
- Try toggling `RELAY_ACTIVE_LOW` setting

## Additional Resources

- [ESP-IDF API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- Main project README for complete documentation

---

**Quick Reference:** To configure WiFi → edit `config.h` → change SSID/password → rebuild → upload
