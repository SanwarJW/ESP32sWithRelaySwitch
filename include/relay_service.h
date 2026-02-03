/**
 * @file relay_service.h
 * @brief Relay control service interface
 * 
 * Provides functions to control relays, manage states, and persist
 * configuration to NVS (Non-Volatile Storage).
 */

#ifndef RELAY_SERVICE_H
#define RELAY_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

/**
 * @brief Relay state enumeration
 */
typedef enum {
    RELAY_OFF = 0,
    RELAY_ON = 1
} relay_state_t;

/**
 * @brief Relay information structure
 */
typedef struct {
    uint8_t gpio_pin;
    const char *name;
    relay_state_t state;
} relay_info_t;

/**
 * @brief Initialize the relay service
 * 
 * Configures GPIO pins, initializes NVS, and restores saved states.
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t relay_service_init(void);

/**
 * @brief Toggle a relay's state
 * 
 * @param relay_id Relay index (0-3)
 * @return New state after toggle, or -1 on error
 */
int relay_toggle(uint8_t relay_id);

/**
 * @brief Set a relay to a specific state
 * 
 * @param relay_id Relay index (0-3)
 * @param state Desired state (RELAY_ON or RELAY_OFF)
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if relay_id is invalid
 */
esp_err_t relay_set_state(uint8_t relay_id, relay_state_t state);

/**
 * @brief Get the current state of a relay
 * 
 * @param relay_id Relay index (0-3)
 * @return Current state, or -1 on error
 */
int relay_get_state(uint8_t relay_id);

/**
 * @brief Get relay information
 * 
 * @param relay_id Relay index (0-3)
 * @return Pointer to relay_info_t structure, or NULL if invalid
 */
const relay_info_t* relay_get_info(uint8_t relay_id);

/**
 * @brief Get the count of relays
 * 
 * @return Number of relays configured
 */
uint8_t relay_get_count(void);

/**
 * @brief Save all relay states to NVS
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t relay_save_states(void);

/**
 * @brief Load relay states from NVS
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t relay_load_states(void);

/**
 * @brief Turn all relays OFF
 * 
 * @return ESP_OK on success
 */
esp_err_t relay_all_off(void);

/**
 * @brief Turn all relays ON
 * 
 * @return ESP_OK on success
 */
esp_err_t relay_all_on(void);

#endif // RELAY_SERVICE_H
