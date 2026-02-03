/**
 * @file relay_service.c
 * @brief Relay control service implementation
 */

#include "relay_service.h"
#include "config.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = LOG_TAG_RELAY;

// Relay configuration array
static relay_info_t relays[RELAY_COUNT] = {
    { .gpio_pin = RELAY_1_GPIO, .name = RELAY_1_NAME, .state = RELAY_DEFAULT_STATE },
    { .gpio_pin = RELAY_2_GPIO, .name = RELAY_2_NAME, .state = RELAY_DEFAULT_STATE },
    { .gpio_pin = RELAY_3_GPIO, .name = RELAY_3_NAME, .state = RELAY_DEFAULT_STATE },
    { .gpio_pin = RELAY_4_GPIO, .name = RELAY_4_NAME, .state = RELAY_DEFAULT_STATE }
};

// Last toggle time for debouncing
static uint32_t last_toggle_time[RELAY_COUNT] = {0};

// LED state
static bool led_initialized = false;

/*============================================================================
 * Private Functions
 *============================================================================*/

/**
 * @brief Initialize the built-in LED
 */
static void init_led(void)
{
    if (led_initialized) return;
    
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << LED_BUILTIN_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_conf);
    gpio_set_level(LED_BUILTIN_GPIO, 0);
    led_initialized = true;
    ESP_LOGI(TAG, "Built-in LED initialized on GPIO %d", LED_BUILTIN_GPIO);
}

/**
 * @brief Blink the built-in LED to indicate state change
 */
static void blink_led(void)
{
    if (!led_initialized) return;
    
    for (int i = 0; i < LED_BLINK_COUNT; i++) {
        gpio_set_level(LED_BUILTIN_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(LED_BLINK_ON_MS));
        gpio_set_level(LED_BUILTIN_GPIO, 0);
        if (i < LED_BLINK_COUNT - 1) {
            vTaskDelay(pdMS_TO_TICKS(LED_BLINK_ON_MS));
        }
    }
}

/**
 * @brief Apply relay state to GPIO
 */
static void apply_gpio_state(uint8_t relay_id)
{
    if (relay_id >= RELAY_COUNT) return;
    
    int gpio_level;
    
#if RELAY_ACTIVE_LOW
    // Active LOW: Relay ON when GPIO is LOW
    gpio_level = (relays[relay_id].state == RELAY_ON) ? 0 : 1;
#else
    // Active HIGH: Relay ON when GPIO is HIGH
    gpio_level = (relays[relay_id].state == RELAY_ON) ? 1 : 0;
#endif
    
    gpio_set_level(relays[relay_id].gpio_pin, gpio_level);
}

/**
 * @brief Check if enough time has passed since last toggle (debounce)
 */
static bool debounce_check(uint8_t relay_id)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if ((now - last_toggle_time[relay_id]) < RELAY_DEBOUNCE_MS) {
        return false;
    }
    last_toggle_time[relay_id] = now;
    return true;
}

/*============================================================================
 * Public Functions
 *============================================================================*/

esp_err_t relay_service_init(void)
{
    ESP_LOGI(TAG, "Initializing relay service...");
    
    // Initialize built-in LED for status indication
    init_led();
    
    // Configure GPIO pins - use OPEN DRAIN for 5V relay module compatibility
    // Open-drain: LOW = 0V (relay ON), HIGH = float (relay module's pull-up pulls to 5V = relay OFF)
    for (int i = 0; i < RELAY_COUNT; i++) {
        // First set GPIO HIGH (will float in open-drain) before configuring
        gpio_set_level(relays[i].gpio_pin, 1);
        
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << relays[i].gpio_pin),
            .mode = GPIO_MODE_OUTPUT_OD,  // Open-drain output for 5V compatibility
            .pull_up_en = GPIO_PULLUP_DISABLE,  // Rely on relay module's pull-up
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        
        esp_err_t ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure GPIO %d: %s", 
                     relays[i].gpio_pin, esp_err_to_name(ret));
            return ret;
        }
        
        // Ensure GPIO is HIGH (floating) - relay OFF
        gpio_set_level(relays[i].gpio_pin, 1);
        
        ESP_LOGI(TAG, "Configured GPIO %d for %s (open-drain)", 
                 relays[i].gpio_pin, relays[i].name);
    }
    
    // Load saved states from NVS if persistence is enabled
#if RELAY_PERSIST_STATE
    esp_err_t ret = relay_load_states();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "No saved states found, using defaults");
    }
#endif
    
    // Apply initial states to all relays
    for (int i = 0; i < RELAY_COUNT; i++) {
        apply_gpio_state(i);
        ESP_LOGI(TAG, "%s initialized: %s", 
                 relays[i].name, 
                 relays[i].state == RELAY_ON ? "ON" : "OFF");
    }
    
    ESP_LOGI(TAG, "Relay service initialized successfully");
    return ESP_OK;
}

int relay_toggle(uint8_t relay_id)
{
    if (relay_id >= RELAY_COUNT) {
        ESP_LOGE(TAG, "Invalid relay ID: %d", relay_id);
        return -1;
    }
    
    // Debounce check
    if (!debounce_check(relay_id)) {
        ESP_LOGW(TAG, "Toggle ignored (debounce): %s", relays[relay_id].name);
        return relays[relay_id].state;
    }
    
    // Toggle state
    relays[relay_id].state = (relays[relay_id].state == RELAY_ON) ? RELAY_OFF : RELAY_ON;
    apply_gpio_state(relay_id);
    
    // Blink LED to indicate change
    blink_led();
    
    ESP_LOGI(TAG, "%s toggled to %s", 
             relays[relay_id].name,
             relays[relay_id].state == RELAY_ON ? "ON" : "OFF");
    
    // Save state to NVS
#if RELAY_PERSIST_STATE
    relay_save_states();
#endif
    
    return relays[relay_id].state;
}

esp_err_t relay_set_state(uint8_t relay_id, relay_state_t state)
{
    if (relay_id >= RELAY_COUNT) {
        ESP_LOGE(TAG, "Invalid relay ID: %d", relay_id);
        return ESP_ERR_INVALID_ARG;
    }
    
    relays[relay_id].state = state;
    apply_gpio_state(relay_id);
    
    // Blink LED to indicate change
    blink_led();
    
    ESP_LOGI(TAG, "%s set to %s", 
             relays[relay_id].name,
             state == RELAY_ON ? "ON" : "OFF");
    
#if RELAY_PERSIST_STATE
    relay_save_states();
#endif
    
    return ESP_OK;
}

int relay_get_state(uint8_t relay_id)
{
    if (relay_id >= RELAY_COUNT) {
        ESP_LOGE(TAG, "Invalid relay ID: %d", relay_id);
        return -1;
    }
    
    return relays[relay_id].state;
}

const relay_info_t* relay_get_info(uint8_t relay_id)
{
    if (relay_id >= RELAY_COUNT) {
        return NULL;
    }
    return &relays[relay_id];
}

uint8_t relay_get_count(void)
{
    return RELAY_COUNT;
}

esp_err_t relay_save_states(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;
    
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Pack all states into a single byte (4 relays = 4 bits)
    uint8_t packed_states = 0;
    for (int i = 0; i < RELAY_COUNT; i++) {
        if (relays[i].state == RELAY_ON) {
            packed_states |= (1 << i);
        }
    }
    
    ret = nvs_set_u8(nvs_handle, NVS_KEY_RELAY_STATE, packed_states);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save states: %s", esp_err_to_name(ret));
        nvs_close(nvs_handle);
        return ret;
    }
    
    ret = nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    
    ESP_LOGD(TAG, "States saved: 0x%02X", packed_states);
    return ret;
}

esp_err_t relay_load_states(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;
    
    ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NVS namespace not found: %s", esp_err_to_name(ret));
        return ret;
    }
    
    uint8_t packed_states = 0;
    ret = nvs_get_u8(nvs_handle, NVS_KEY_RELAY_STATE, &packed_states);
    nvs_close(nvs_handle);
    
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "No saved states found: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Unpack states
    for (int i = 0; i < RELAY_COUNT; i++) {
        relays[i].state = (packed_states & (1 << i)) ? RELAY_ON : RELAY_OFF;
    }
    
    ESP_LOGI(TAG, "States loaded: 0x%02X", packed_states);
    return ESP_OK;
}

esp_err_t relay_all_off(void)
{
    ESP_LOGI(TAG, "Turning all relays OFF");
    for (int i = 0; i < RELAY_COUNT; i++) {
        relays[i].state = RELAY_OFF;
        apply_gpio_state(i);
    }
    
#if RELAY_PERSIST_STATE
    relay_save_states();
#endif
    
    return ESP_OK;
}

esp_err_t relay_all_on(void)
{
    ESP_LOGI(TAG, "Turning all relays ON");
    for (int i = 0; i < RELAY_COUNT; i++) {
        relays[i].state = RELAY_ON;
        apply_gpio_state(i);
    }
    
#if RELAY_PERSIST_STATE
    relay_save_states();
#endif
    
    return ESP_OK;
}
