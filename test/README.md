# Test Directory

This directory is designated for PlatformIO Unit Testing and project tests for the ESP32 Relay Controller.

## Overview

Unit testing allows you to verify individual components of your firmware work correctly in isolation. This helps:
- Catch bugs early in development
- Ensure code changes don't break existing functionality
- Document expected behavior through test cases
- Improve code quality and maintainability

## Current Status

**This project currently has no unit tests implemented.** The directory is prepared for future test development.

## Why Unit Testing for Embedded Systems?

Embedded testing is crucial because:
- Hardware bugs are expensive to debug
- Real-time systems have strict timing requirements
- Remote deployments make updates difficult
- Safety-critical applications need verification

## PlatformIO Unit Testing Framework

PlatformIO provides a built-in testing framework that supports:
- **Native tests:** Run on your development machine (fast, no hardware)
- **Embedded tests:** Run on actual ESP32 hardware (realistic, complete)
- **Desktop simulation:** Test with simulators

## Getting Started with Tests

### Test Structure

Tests should be placed in subdirectories:

```
test/
├── README.md                    # This file
├── test_relay_service/
│   └── test_relay_service.c    # Tests for relay_service
├── test_wifi_service/
│   └── test_wifi_service.c     # Tests for wifi_service
└── test_http_controller/
    └── test_http_controller.c  # Tests for http_controller
```

### Example Test File

Create `test/test_relay_service/test_relay_service.c`:

```c
#include <unity.h>
#include "relay_service.h"

// Run before each test
void setUp(void) {
    relay_service_init();
}

// Run after each test
void tearDown(void) {
    // Cleanup if needed
}

// Test case: Initial state should be OFF
void test_relay_initial_state(void) {
    relay_state_t state = relay_get_state(0);
    TEST_ASSERT_EQUAL(RELAY_OFF, state);
}

// Test case: Toggle changes state
void test_relay_toggle(void) {
    relay_set_state(0, RELAY_OFF);
    relay_toggle(0);
    TEST_ASSERT_EQUAL(RELAY_ON, relay_get_state(0));
    
    relay_toggle(0);
    TEST_ASSERT_EQUAL(RELAY_OFF, relay_get_state(0));
}

// Test case: Invalid relay ID returns error
void test_relay_invalid_id(void) {
    int result = relay_toggle(99);
    TEST_ASSERT_EQUAL(-1, result);
}

// Test case: All relays control
void test_relay_all_control(void) {
    relay_all_on();
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL(RELAY_ON, relay_get_state(i));
    }
    
    relay_all_off();
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL(RELAY_OFF, relay_get_state(i));
    }
}

// Main test entry point
void app_main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_relay_initial_state);
    RUN_TEST(test_relay_toggle);
    RUN_TEST(test_relay_invalid_id);
    RUN_TEST(test_relay_all_control);
    
    UNITY_END();
}
```

## Running Tests

### Run All Tests

```bash
# Run tests on ESP32 hardware
pio test

# Run specific test
pio test -f test_relay_service

# Run with verbose output
pio test -v
```

### Run on Native (Desktop)

For tests that don't need hardware:

```ini
; Add to platformio.ini
[env:native]
platform = native
test_framework = unity
```

```bash
pio test -e native
```

## Unity Testing Framework

PlatformIO uses Unity - a lightweight C testing framework.

### Common Assertions

```c
// Basic assertions
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);
TEST_ASSERT(condition);

// Equality
TEST_ASSERT_EQUAL(expected, actual);
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_EQUAL_UINT8(expected, actual);
TEST_ASSERT_EQUAL_STRING(expected, actual);

// Comparison
TEST_ASSERT_GREATER_THAN(threshold, actual);
TEST_ASSERT_LESS_THAN(threshold, actual);

// Null/Not Null
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);

// Floating point
TEST_ASSERT_EQUAL_FLOAT(expected, actual);
TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual);

// Memory
TEST_ASSERT_EQUAL_MEMORY(expected, actual, length);

// Custom messages
TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Custom error message");
```

## Test Organization

### Test Naming Convention

- Prefix test functions with `test_`
- Use descriptive names: `test_relay_toggle_from_off_to_on`
- Group related tests in the same file

### Test Categories

#### Unit Tests
Test individual functions in isolation:
- `test_relay_set_state`
- `test_relay_get_info`
- `test_relay_validate_id`

#### Integration Tests
Test multiple components together:
- `test_relay_with_nvs_persistence`
- `test_http_endpoint_toggle_relay`
- `test_wifi_reconnect_with_static_ip`

#### Hardware Tests
Test with actual GPIO and hardware:
- `test_relay_gpio_output`
- `test_led_blink_on_state_change`

## Example Test Scenarios for This Project

### 1. Relay Service Tests

```c
// State management
test_relay_initial_state_is_off()
test_relay_set_state_on()
test_relay_set_state_off()
test_relay_toggle_changes_state()

// Persistence
test_relay_state_saved_to_nvs()
test_relay_state_restored_from_nvs()

// Validation
test_relay_invalid_id_returns_error()
test_relay_info_contains_correct_gpio()

// Bulk operations
test_relay_all_on()
test_relay_all_off()
```

### 2. WiFi Service Tests

```c
// Connection
test_wifi_init_succeeds()
test_wifi_connects_with_valid_credentials()
test_wifi_fails_with_invalid_credentials()
test_wifi_returns_correct_ip_address()

// State
test_wifi_is_connected_returns_true_when_connected()
test_wifi_is_connected_returns_false_when_disconnected()

// Reconnection
test_wifi_reconnects_after_disconnect()
```

### 3. HTTP Controller Tests

```c
// Endpoint responses
test_http_get_root_returns_html()
test_http_toggle_endpoint_changes_relay()
test_http_status_endpoint_returns_json()

// Error handling
test_http_invalid_relay_id_returns_400()
test_http_malformed_request_handled()

// Server lifecycle
test_http_server_starts_successfully()
test_http_server_handle_not_null()
```

## Mocking for Unit Tests

For true unit tests, mock dependencies:

### Example: Mock NVS for Testing

```c
// Mock NVS functions
esp_err_t __wrap_nvs_get_u8(nvs_handle_t handle, const char* key, uint8_t* out_value) {
    *out_value = 0; // Return OFF state
    return ESP_OK;
}

esp_err_t __wrap_nvs_set_u8(nvs_handle_t handle, const char* key, uint8_t value) {
    // Just return success
    return ESP_OK;
}
```

Configure in `platformio.ini`:

```ini
[env:esp32dev]
test_build_flags =
    -Wl,--wrap=nvs_get_u8
    -Wl,--wrap=nvs_set_u8
```

## Test-Driven Development (TDD)

TDD cycle for embedded development:

1. **Write a failing test** that defines desired behavior
2. **Write minimal code** to make the test pass
3. **Refactor** while keeping tests passing
4. **Repeat** for next feature

### Example TDD Workflow

```bash
# 1. Write test (it fails)
# Edit test/test_relay_service/test_relay_service.c
pio test -f test_relay_service  # ❌ FAIL

# 2. Implement feature
# Edit src/relay_service.c
pio test -f test_relay_service  # ✅ PASS

# 3. Refactor if needed
# Edit src/relay_service.c
pio test -f test_relay_service  # ✅ PASS
```

## Coverage Analysis

Check how much code is tested:

```ini
[env:native]
platform = native
test_framework = unity
build_flags = --coverage
extra_scripts = post:coverage.py
```

Run coverage:

```bash
pio test -e native
lcov --directory .pio/build/native --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Continuous Integration

Automate testing with GitHub Actions:

```yaml
# .github/workflows/test.yml
name: PlatformIO CI

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-python@v2
      - name: Install PlatformIO
        run: pip install platformio
      - name: Run tests
        run: pio test -e native
```

## Best Practices

### Do's
- ✅ Write tests for critical functionality first
- ✅ Keep tests simple and focused (one concept per test)
- ✅ Use descriptive test names
- ✅ Test edge cases and error conditions
- ✅ Run tests before committing code
- ✅ Mock external dependencies when possible

### Don'ts
- ❌ Don't test ESP-IDF framework code (trust it works)
- ❌ Don't make tests dependent on each other
- ❌ Don't test implementation details (test behavior)
- ❌ Don't skip tests that fail (fix them!)
- ❌ Don't write tests that depend on timing

## Debugging Tests

### View Test Output

```bash
# Verbose mode shows more details
pio test -v

# Filter specific test
pio test -f test_relay_service -v

# Run on hardware with monitor
pio test --upload-port /dev/cu.usbserial-0001 -v
```

### Common Issues

**Test hangs:**
- Check for infinite loops
- Verify hardware connections
- Increase timeout in `platformio.ini`

**Test fails randomly:**
- Timing issues (add delays)
- Uninitialized variables
- Hardware state not reset between tests

## Performance Testing

Test timing requirements:

```c
void test_relay_toggle_speed(void) {
    uint32_t start = esp_timer_get_time();
    relay_toggle(0);
    uint32_t duration = esp_timer_get_time() - start;
    
    // Should complete in under 1ms
    TEST_ASSERT_LESS_THAN(1000, duration);
}
```

## Integration with IDE

### VS Code with PlatformIO

1. Install PlatformIO IDE extension
2. Open project
3. Click **Test** icon in sidebar
4. Run tests with one click

## Resources

- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [Unity Test Framework](http://www.throwtheswitch.org/unity)
- [ESP-IDF Unit Testing](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/unit-tests.html)
- [Test-Driven Development for Embedded C](https://pragprog.com/titles/jgade/test-driven-development-for-embedded-c/)

## Next Steps

To start testing this project:

1. **Pick a module** to test (e.g., relay_service)
2. **Create test directory**: `test/test_relay_service/`
3. **Write first test**: Start simple (test initialization)
4. **Run the test**: `pio test -f test_relay_service`
5. **Expand coverage**: Add more test cases
6. **Automate**: Set up CI/CD pipeline

## Example: Complete Test File

See example above in "Example Test File" section for a complete working test implementation.

## Summary

Unit testing is essential for robust embedded systems. While this project doesn't currently have tests, the structure is ready. Start with critical components like relay_service and wifi_service, then expand coverage over time.

**Remember:** Tests are code too - keep them clean, maintainable, and documented!

---

**Quick Start:** Create `test/test_relay_service/test_relay_service.c` → Write tests → Run `pio test` → Fix failures → Celebrate! 🎉
