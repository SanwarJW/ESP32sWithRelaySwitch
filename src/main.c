/**
 * @file main.c
 * @brief ESP32 Relay Controller - Main Entry Point
 * 
 * This application provides a web-based interface to control 4 relays
 * connected to lights and fans.
 * 
 * Features:
 *   - REST API for relay control
 *   - Web UI with toggle buttons
 *   - State persistence across reboots
 *   - Configurable parameters
 * 
 * @author ESP32 Relay Controller
 * @version 1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "config.h"
#include "wifi_service.h"
#include "relay_service.h"
#include "http_controller.h"

static const char *TAG = LOG_TAG_MAIN;

/**
 * @brief Initialize NVS (Non-Volatile Storage)
 */
static esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    return ret;
}

/**
 * @brief Print startup banner
 */
static void print_banner(void)
{
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     ESP32 Relay Controller v1.0       ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  Relays: 4 (2 Lights, 2 Fans)         ║\n");
    printf("║  GPIO:   16, 17, 18, 19               ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * @brief Main application entry point
 */
void app_main(void)
{
    print_banner();
    
    ESP_LOGI(TAG, "=== Starting ESP32 Relay Controller ===");
    
    // Step 1: Initialize NVS (required for WiFi and state persistence)
    ESP_LOGI(TAG, "[1/4] Initializing NVS...");
    ESP_ERROR_CHECK(init_nvs());
    ESP_LOGI(TAG, "NVS initialized");
    
    // Step 2: Initialize relay service
    ESP_LOGI(TAG, "[2/4] Initializing relay service...");
    ESP_ERROR_CHECK(relay_service_init());
    ESP_LOGI(TAG, "Relay service initialized");
    
    // Step 3: Connect to WiFi
    ESP_LOGI(TAG, "[3/4] Connecting to WiFi...");
    esp_err_t wifi_ret = wifi_service_init();
    if (wifi_ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi connection failed! Restarting in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
    }
    ESP_LOGI(TAG, "WiFi connected");
    
    // Step 4: Start HTTP server
    ESP_LOGI(TAG, "[4/4] Starting HTTP server...");
    ESP_ERROR_CHECK(http_controller_init());
    ESP_LOGI(TAG, "HTTP server started");
    
    // Print access information
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║          SYSTEM READY                 ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  Open in browser:                     ║\n");
    printf("║  http://%-30s║\n", wifi_get_ip_address());
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  API Endpoints:                       ║\n");
    printf("║  GET /relay/{0-3}/toggle              ║\n");
    printf("║  GET /relay/{0-3}/status              ║\n");
    printf("║  GET /relay/{0-3}/on                  ║\n");
    printf("║  GET /relay/{0-3}/off                 ║\n");
    printf("║  GET /relay/all/on                    ║\n");
    printf("║  GET /relay/all/off                   ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n");
    
    ESP_LOGI(TAG, "=== System running ===");
    
    // Main loop - can be used for status LED or watchdog
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Sleep 10 seconds
        
        // Optional: Log status periodically
        ESP_LOGD(TAG, "System running, WiFi: %s, IP: %s",
                 wifi_is_connected() ? "connected" : "disconnected",
                 wifi_get_ip_address());
    }
}