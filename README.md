# ESP32 Relay Controller

A professional-grade ESP32 firmware for controlling 4 relays via a web interface and REST API. Perfect for home automation, controlling lights, fans, and other appliances.

## Features

- 🌐 **Web-based UI** - Clean, responsive interface with toggle buttons
- 🔌 **REST API** - Full control via HTTP endpoints
- 💾 **State Persistence** - Relay states saved to NVS (survives reboots)
- 📡 **Auto WiFi Reconnection** - Automatic recovery from network issues
- 🔄 **HTTP Watchdog** - Monitors and restarts server if needed
- ⚙️ **Configurable** - Easy-to-modify parameters in `config.h`
- 🛡️ **Safe Defaults** - All relays OFF on boot (before loading saved state)

## Hardware Requirements

- **ESP32 DevKit** (or compatible board)
- **4-Channel Relay Module** (5V with optocoupler isolation)
- **Power Supply** (5V for relay module, USB for ESP32)

### Wiring Diagram

```
ESP32 DevKit          4-Channel Relay Module
--------------        ----------------------
GPIO 16       -----> IN1 (Relay 1 - Light 1)
GPIO 17       -----> IN2 (Relay 2 - Light 2)
GPIO 18       -----> IN3 (Relay 3 - Fan 1)
GPIO 19       -----> IN4 (Relay 4 - Fan 2)
GND           -----> GND
                      VCC (Connect to 5V supply)
```

**Note:** Most relay modules are **active LOW** (LOW = ON). This is configured in `config.h`.

## Software Requirements

- [PlatformIO](https://platformio.org/) (CLI or IDE extension)
- ESP-IDF framework (automatically installed by PlatformIO)
- USB drivers for ESP32 (CH340/CP2102)

## Quick Start

### 1. Clone and Configure

```bash
git clone <repository-url>
cd ESP32WithRelaySwitch
```

### 2. Edit WiFi Settings

Open `include/config.h` and update:

```c
#define WIFI_SSID           "YourWiFiName"
#define WIFI_PASSWORD       "YourPassword"
#define STATIC_IP           "192.168.1.100"  // Change as needed
#define STATIC_GATEWAY      "192.168.1.1"    // Your router's IP
```

### 3. Build and Upload

```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

Or use PlatformIO IDE:
- Click **Build** (✓)
- Click **Upload** (→)
- Click **Monitor** (🔌)

### 4. Access the Interface

After successful boot, open your browser to:

```
http://192.168.1.100  (or your configured IP)
```

## API Documentation

### Control Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/` | Web UI home page |
| GET | `/relay/{0-3}/toggle` | Toggle specific relay |
| GET | `/relay/{0-3}/on` | Turn relay ON |
| GET | `/relay/{0-3}/off` | Turn relay OFF |
| GET | `/relay/{0-3}/status` | Get relay state (JSON) |
| GET | `/relay/all/on` | Turn all relays ON |
| GET | `/relay/all/off` | Turn all relays OFF |

### API Examples

```bash
# Toggle Light 1 (Relay 0)
curl http://192.168.1.100/relay/0/toggle

# Turn on Fan 1 (Relay 2)
curl http://192.168.1.100/relay/2/on

# Get status of Light 2 (Relay 1)
curl http://192.168.1.100/relay/1/status
# Response: {"relay":1,"state":"on","name":"Light 2"}

# Turn off all relays
curl http://192.168.1.100/relay/all/off
```

## Configuration

All settings are in [`include/config.h`](include/config.h):

### WiFi Settings
- `WIFI_SSID` - Your WiFi network name
- `WIFI_PASSWORD` - Your WiFi password
- `USE_STATIC_IP` - Set to `1` for static IP, `0` for DHCP
- `STATIC_IP`, `STATIC_GATEWAY`, `STATIC_SUBNET` - Network settings

### Relay Configuration
- `RELAY_X_GPIO` - GPIO pin assignments (16, 17, 18, 19)
- `RELAY_X_NAME` - Display names for web UI
- `RELAY_ACTIVE_LOW` - Set to `1` for active LOW relays (default)
- `RELAY_DEFAULT_STATE` - Initial state on boot (0 = OFF)
- `RELAY_PERSIST_STATE` - Enable state persistence (1 = enabled)

### HTTP Server
- `HTTP_MAX_CONNECTIONS` - Max simultaneous connections (1-7)
- `HTTP_KEEP_ALIVE` - Enable persistent connections
- `HTTP_TASK_PRIORITY` - Server task priority (1-24)
- `HTTP_TASK_STACK_SIZE` - Server task stack size (bytes)

## Project Structure

```
ESP32WithRelaySwitch/
├── README.md                    # This file
├── platformio.ini               # PlatformIO configuration
├── include/                     # Header files
│   ├── README                   # Header files documentation
│   ├── config.h                 # Main configuration file
│   ├── relay_service.h          # Relay control service interface
│   ├── wifi_service.h           # WiFi management interface
│   ├── http_controller.h        # HTTP server interface
│   └── ui_templates.h           # HTML templates
├── src/                         # Source files
│   ├── main.c                   # Application entry point
│   ├── relay_service.c          # Relay control implementation
│   ├── wifi_service.c           # WiFi management
│   └── http_controller.c        # HTTP server & API handlers
├── lib/                         # External libraries (if any)
│   └── README                   # Libraries documentation
└── test/                        # Unit tests
    └── README                   # Testing documentation
```

## Troubleshooting

### WiFi Connection Issues

**Problem:** ESP32 can't connect to WiFi

**Solution:**
1. Double-check SSID and password in `config.h`
2. Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
3. Check WiFi signal strength
4. Try disabling static IP (`USE_STATIC_IP = 0`)

### Relays Not Switching

**Problem:** Web UI works but relays don't respond

**Solution:**
1. Verify wiring connections
2. Check if relay module is powered (separate 5V supply)
3. Try changing `RELAY_ACTIVE_LOW` to `0` in `config.h`
4. Test GPIO pins with a multimeter

### Cannot Upload to ESP32

**Problem:** Upload fails with timeout or connection errors

**Solution:**
1. Hold **BOOT** button while uploading
2. Check USB cable (use data cable, not charge-only)
3. Install correct USB drivers (CH340 or CP2102)
4. Select correct COM port in PlatformIO

### HTTP Server Crashes

**Problem:** Server stops responding after some time

**Solution:**
1. Increase `HTTP_TASK_STACK_SIZE` in `config.h`
2. Reduce `HTTP_MAX_CONNECTIONS` to conserve memory
3. Check serial monitor for error messages
4. Enable watchdog recovery (already implemented)

## Serial Monitor Output

Example startup sequence:

```
╔═══════════════════════════════════════╗
║     ESP32 Relay Controller v1.0       ║
╠═══════════════════════════════════════╣
║  Relays: 4 (2 Lights, 2 Fans)         ║
║  GPIO:   16, 17, 18, 19               ║
╚═══════════════════════════════════════╝

[MAIN] [1/4] Initializing NVS...
[MAIN] NVS initialized
[MAIN] [2/4] Initializing relay service...
[RELAY] Loaded relay 0 state: OFF
[RELAY] Loaded relay 1 state: OFF
[MAIN] Relay service initialized
[MAIN] [3/4] Connecting to WiFi...
[WIFI] Connected to FTTH
[WIFI] IP Address: 192.168.1.100
[MAIN] WiFi connected
[MAIN] [4/4] Starting HTTP server...
[HTTP] Server started
[MAIN] HTTP server started

╔═══════════════════════════════════════╗
║          SYSTEM READY                 ║
╠═══════════════════════════════════════╣
║  Open in browser:                     ║
║  http://192.168.1.100                 ║
╠═══════════════════════════════════════╣
║  API Endpoints:                       ║
║  GET /relay/{0-3}/toggle              ║
║  GET /relay/{0-3}/status              ║
║  GET /relay/{0-3}/on                  ║
║  GET /relay/{0-3}/off                 ║
║  GET /relay/all/on                    ║
║  GET /relay/all/off                   ║
╚═══════════════════════════════════════╝
```

## Advanced Features

### State Persistence

Relay states are automatically saved to NVS flash memory and restored on boot:
- Survives power outages and reboots
- No external EEPROM needed
- Can be disabled in `config.h`

### Auto-Recovery

The system monitors health and recovers automatically:
- WiFi disconnection: Restarts ESP32 after 5 minutes
- HTTP server crash: Restarts server automatically
- Status checks every 10 seconds

### Customization

You can easily customize:
- Number of relays (up to 8 with code changes)
- GPIO pin assignments
- Relay names and types
- Network settings
- Performance parameters

## Development

### Building from Source

```bash
# Install PlatformIO
pip install platformio

# Build
pio run

# Clean build
pio run --target clean
pio run

# Upload and monitor
pio run --target upload && pio device monitor
```

### Code Style

- C99 standard
- Doxygen-style comments
- ESP-IDF logging macros
- Error checking on all ESP functions

### Adding More Relays

1. Define new GPIO in `config.h`:
   ```c
   #define RELAY_5_GPIO    21
   #define RELAY_5_NAME    "Pump"
   ```
2. Increment `RELAY_COUNT`
3. Add GPIO to array in `relay_service.c`

## License

This project is open source. Feel free to modify and distribute.

## Support

For issues and questions:
- Check the troubleshooting section above
- Review serial monitor output for error messages
- Consult ESP-IDF documentation

## Version History

- **v1.1** - Added HTTP watchdog, improved WiFi recovery
- **v1.0** - Initial release with web UI and REST API

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test your changes
4. Submit a pull request

---

**Made with ❤️ using ESP-IDF and PlatformIO**
