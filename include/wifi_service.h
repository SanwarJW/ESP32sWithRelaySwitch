/**
 * @file wifi_service.h
 * @brief WiFi connection service interface
 */

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initialize and connect to WiFi
 * 
 * Blocks until connected or max retries exceeded.
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_service_init(void);

/**
 * @brief Check if WiFi is connected
 * 
 * @return true if connected, false otherwise
 */
bool wifi_is_connected(void);

/**
 * @brief Get the IP address as a string
 * 
 * @return IP address string (e.g., "192.168.1.100")
 */
const char* wifi_get_ip_address(void);

/**
 * @brief Disconnect from WiFi
 */
void wifi_disconnect(void);

#endif // WIFI_SERVICE_H
