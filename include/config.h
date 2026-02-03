/**
 * @file config.h
 * @brief Configuration file for ESP32 Relay Controller
 * 
 * This file contains all tunable parameters for the relay control system.
 * Modify these values to customize behavior without changing core logic.
 */

#ifndef CONFIG_H
#define CONFIG_H

/*============================================================================
 * WiFi Configuration
 *============================================================================*/
#define WIFI_SSID           "FTTH"
#define WIFI_PASSWORD       "bsnl@7979"
#define WIFI_MAX_RETRY      10          // Number of reconnection attempts
#define WIFI_RETRY_DELAY_MS 1000        // Delay between retries (milliseconds)

/*============================================================================
 * Static IP Configuration (set USE_STATIC_IP to 1 to enable)
 *============================================================================*/
#define USE_STATIC_IP       1           // 0 = DHCP (dynamic), 1 = Static IP
#define STATIC_IP           "192.168.1.100"   // Your desired static IP
#define STATIC_GATEWAY      "192.168.1.1"     // Your router's IP
#define STATIC_SUBNET       "255.255.255.0"   // Subnet mask
#define STATIC_DNS          "8.8.8.8"         // DNS server (Google DNS)

/*============================================================================
 * Built-in LED Configuration (for status indication)
 *============================================================================*/
#define LED_BUILTIN_GPIO    2           // ESP32 DevKit built-in LED
#define LED_BLINK_ON_MS     50          // LED on duration (ms)
#define LED_BLINK_COUNT     1           // Number of blinks on state change

/*============================================================================
 * Relay GPIO Configuration
 * 
 * Wiring:
 *   GPIO 16 -> IN1 (Relay 1 - Light 1)
 *   GPIO 17 -> IN2 (Relay 2 - Light 2)
 *   GPIO 18 -> IN3 (Relay 3 - Fan 1)
 *   GPIO 19 -> IN4 (Relay 4 - Fan 2)
 *============================================================================*/
#define RELAY_COUNT         4

#define RELAY_1_GPIO        16          // Light 1
#define RELAY_2_GPIO        17          // Light 2
#define RELAY_3_GPIO        18          // Fan 1
#define RELAY_4_GPIO        19          // Fan 2

// Relay names for UI display
#define RELAY_1_NAME        "Light 1"
#define RELAY_2_NAME        "Light 2"
#define RELAY_3_NAME        "Fan 1"
#define RELAY_4_NAME        "Fan 2"

/*============================================================================
 * Relay Behavior Configuration
 *============================================================================*/
// Active LOW: GPIO LOW = Relay ON (common for most relay modules)
// Active HIGH: GPIO HIGH = Relay ON
#define RELAY_ACTIVE_LOW    1           // Set to 0 for active HIGH relays

// Default state when ESP32 boots (before loading saved state)
// 0 = OFF, 1 = ON
#define RELAY_DEFAULT_STATE 0           // OFF for safety

// Save relay states to flash (NVS) for persistence across reboots
#define RELAY_PERSIST_STATE 1           // Set to 0 to disable

/*============================================================================
 * HTTP Server Configuration
 *============================================================================*/
// Keep-Alive: Reuses TCP connections for faster repeated requests
// Trade-off: Uses more memory per connection but faster response
#define HTTP_KEEP_ALIVE     1           // Set to 0 to disable

// Maximum simultaneous connections (1-7)
// Trade-off: More connections = more RAM usage (~2KB each)
#define HTTP_MAX_CONNECTIONS 4

// Server task priority (1-24, higher = more responsive)
// Trade-off: Too high may starve other tasks
#define HTTP_TASK_PRIORITY  5

// Server task stack size in bytes (2048-8192)
// Trade-off: Larger = handles complex requests, uses more RAM
#define HTTP_TASK_STACK_SIZE 8192

// URI buffer size for incoming requests
#define HTTP_URI_BUFFER_SIZE 512

/*============================================================================
 * Performance Tuning
 *============================================================================*/
// Response buffer size for HTTP responses
#define HTTP_RESPONSE_BUFFER_SIZE 256

// Debounce time for relay switching (prevents rapid toggling)
#define RELAY_DEBOUNCE_MS   50

/*============================================================================
 * NVS (Non-Volatile Storage) Configuration
 *============================================================================*/
#define NVS_NAMESPACE       "relay_ctrl"
#define NVS_KEY_RELAY_STATE "relay_state"

/*============================================================================
 * Logging Configuration
 *============================================================================*/
#define LOG_TAG_MAIN        "MAIN"
#define LOG_TAG_WIFI        "WIFI"
#define LOG_TAG_RELAY       "RELAY"
#define LOG_TAG_HTTP        "HTTP"

#endif // CONFIG_H
