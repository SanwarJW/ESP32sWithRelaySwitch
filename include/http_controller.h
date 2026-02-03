/**
 * @file http_controller.h
 * @brief HTTP server and REST API controller interface
 */

#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Initialize and start the HTTP server
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t http_controller_init(void);

/**
 * @brief Stop the HTTP server
 * 
 * @return ESP_OK on success
 */
esp_err_t http_controller_stop(void);

/**
 * @brief Get the HTTP server handle
 * 
 * @return Server handle or NULL if not running
 */
httpd_handle_t http_controller_get_handle(void);

#endif // HTTP_CONTROLLER_H
