/**
 * @file http_controller.c
 * @brief HTTP server and REST API controller implementation
 * 
 * REST API Endpoints:
 *   GET /              - Home page with UI
 *   GET /relay/{id}/toggle  - Toggle relay and return new state
 *   GET /relay/{id}/status  - Get relay status
 *   GET /relay/{id}/on      - Turn relay ON
 *   GET /relay/{id}/off     - Turn relay OFF
 *   GET /relay/all/status   - Get all relay statuses
 *   GET /relay/all/on       - Turn all relays ON
 *   GET /relay/all/off      - Turn all relays OFF
 */

#include "http_controller.h"
#include "relay_service.h"
#include "wifi_service.h"
#include "ui_templates.h"
#include "config.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = LOG_TAG_HTTP;
static httpd_handle_t s_server = NULL;

/*============================================================================
 * Helper Functions
 *============================================================================*/

/**
 * @brief Extract relay ID from URI path
 */
static int extract_relay_id(const char *uri)
{
    // Expected format: /relay/{id}/action
    const char *p = strstr(uri, "/relay/");
    if (p == NULL) return -1;
    
    p += 7; // Skip "/relay/"
    
    // Check for "all" keyword
    if (strncmp(p, "all", 3) == 0) {
        return -2; // Special value for "all"
    }
    
    int id = atoi(p);
    if (id < 0 || id >= RELAY_COUNT) {
        return -1;
    }
    
    return id;
}

/**
 * @brief Send JSON response
 */
static esp_err_t send_json_response(httpd_req_t *req, const char *json)
{
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    
#if HTTP_KEEP_ALIVE
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
#endif
    
    return httpd_resp_sendstr(req, json);
}

/*============================================================================
 * Route Handlers
 *============================================================================*/

/**
 * @brief Home page handler - serves the UI
 */
static esp_err_t handler_home(httpd_req_t *req)
{
    ESP_LOGI(TAG, "GET /");
    
    // Build response with current relay states
    char response[3200];
    
    int s0 = relay_get_state(0);
    int s1 = relay_get_state(1);
    int s2 = relay_get_state(2);
    int s3 = relay_get_state(3);
    
    snprintf(response, sizeof(response), HTML_PAGE,
        wifi_get_ip_address(),
        s0 ? "on" : "off", s0 ? "ON" : "OFF",
        s1 ? "on" : "off", s1 ? "ON" : "OFF",
        s2 ? "on" : "off", s2 ? "ON" : "OFF",
        s3 ? "on" : "off", s3 ? "ON" : "OFF"
    );
    
    httpd_resp_set_type(req, "text/html");
    
#if HTTP_KEEP_ALIVE
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
#endif
    
    return httpd_resp_sendstr(req, response);
}

/**
 * @brief Toggle relay handler
 */
static esp_err_t handler_toggle(httpd_req_t *req)
{
    int relay_id = extract_relay_id(req->uri);
    
    if (relay_id < 0) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Invalid relay ID");
        httpd_resp_set_status(req, "400 Bad Request");
        return send_json_response(req, error);
    }
    
    ESP_LOGI(TAG, "GET /relay/%d/toggle", relay_id);
    
    int new_state = relay_toggle(relay_id);
    const relay_info_t *info = relay_get_info(relay_id);
    if (info == NULL) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Relay not found");
        httpd_resp_set_status(req, "404 Not Found");
        return send_json_response(req, error);
    }
    
    char response[HTTP_RESPONSE_BUFFER_SIZE];
    snprintf(response, sizeof(response), JSON_RELAY_STATUS,
        relay_id, info->name, new_state);
    
    return send_json_response(req, response);
}

/**
 * @brief Get relay status handler
 */
static esp_err_t handler_status(httpd_req_t *req)
{
    int relay_id = extract_relay_id(req->uri);
    
    if (relay_id == -2) {
        // All relays status
        ESP_LOGI(TAG, "GET /relay/all/status");
        
        char response[512];
        char *p = response;
        
        p += sprintf(p, "%s", JSON_ALL_STATUS_START);
        
        for (int i = 0; i < RELAY_COUNT; i++) {
            const relay_info_t *info = relay_get_info(i);
            if (i > 0) *p++ = ',';
            p += sprintf(p, JSON_RELAY_STATUS, i, info->name, info->state);
        }
        
        sprintf(p, "%s", JSON_ALL_STATUS_END);
        
        return send_json_response(req, response);
    }
    
    if (relay_id < 0) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Invalid relay ID");
        httpd_resp_set_status(req, "400 Bad Request");
        return send_json_response(req, error);
    }
    
    ESP_LOGI(TAG, "GET /relay/%d/status", relay_id);
    
    const relay_info_t *info = relay_get_info(relay_id);
    if (info == NULL) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Relay not found");
        httpd_resp_set_status(req, "404 Not Found");
        return send_json_response(req, error);
    }
    
    char response[HTTP_RESPONSE_BUFFER_SIZE];
    snprintf(response, sizeof(response), JSON_RELAY_STATUS,
        relay_id, info->name, info->state);
    
    return send_json_response(req, response);
}

/**
 * @brief Turn relay ON handler
 */
static esp_err_t handler_on(httpd_req_t *req)
{
    int relay_id = extract_relay_id(req->uri);
    
    if (relay_id == -2) {
        // All relays ON
        ESP_LOGI(TAG, "GET /relay/all/on");
        relay_all_on();
        
        char response[64];
        snprintf(response, sizeof(response), JSON_SUCCESS, "All relays ON");
        return send_json_response(req, response);
    }
    
    if (relay_id < 0) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Invalid relay ID");
        httpd_resp_set_status(req, "400 Bad Request");
        return send_json_response(req, error);
    }
    
    ESP_LOGI(TAG, "GET /relay/%d/on", relay_id);
    
    relay_set_state(relay_id, RELAY_ON);
    const relay_info_t *info = relay_get_info(relay_id);
    if (info == NULL) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Relay not found");
        httpd_resp_set_status(req, "404 Not Found");
        return send_json_response(req, error);
    }
    
    char response[HTTP_RESPONSE_BUFFER_SIZE];
    snprintf(response, sizeof(response), JSON_RELAY_STATUS,
        relay_id, info->name, RELAY_ON);
    
    return send_json_response(req, response);
}

/**
 * @brief Turn relay OFF handler
 */
static esp_err_t handler_off(httpd_req_t *req)
{
    int relay_id = extract_relay_id(req->uri);
    
    if (relay_id == -2) {
        // All relays OFF
        ESP_LOGI(TAG, "GET /relay/all/off");
        relay_all_off();
        
        char response[64];
        snprintf(response, sizeof(response), JSON_SUCCESS, "All relays OFF");
        return send_json_response(req, response);
    }
    
    if (relay_id < 0) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Invalid relay ID");
        httpd_resp_set_status(req, "400 Bad Request");
        return send_json_response(req, error);
    }
    
    ESP_LOGI(TAG, "GET /relay/%d/off", relay_id);
    
    relay_set_state(relay_id, RELAY_OFF);
    const relay_info_t *info = relay_get_info(relay_id);
    if (info == NULL) {
        char error[64];
        snprintf(error, sizeof(error), JSON_ERROR, "Relay not found");
        httpd_resp_set_status(req, "404 Not Found");
        return send_json_response(req, error);
    }
    
    char response[HTTP_RESPONSE_BUFFER_SIZE];
    snprintf(response, sizeof(response), JSON_RELAY_STATUS,
        relay_id, info->name, RELAY_OFF);
    
    return send_json_response(req, response);
}

/*============================================================================
 * URI Registration
 *============================================================================*/

static const httpd_uri_t uri_home = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = handler_home,
    .user_ctx  = NULL
};

// Register separate URI handlers for each relay to ensure proper matching
static const httpd_uri_t uri_toggle_0 = { .uri = "/relay/0/toggle", .method = HTTP_GET, .handler = handler_toggle, .user_ctx = NULL };
static const httpd_uri_t uri_toggle_1 = { .uri = "/relay/1/toggle", .method = HTTP_GET, .handler = handler_toggle, .user_ctx = NULL };
static const httpd_uri_t uri_toggle_2 = { .uri = "/relay/2/toggle", .method = HTTP_GET, .handler = handler_toggle, .user_ctx = NULL };
static const httpd_uri_t uri_toggle_3 = { .uri = "/relay/3/toggle", .method = HTTP_GET, .handler = handler_toggle, .user_ctx = NULL };

static const httpd_uri_t uri_status_0 = { .uri = "/relay/0/status", .method = HTTP_GET, .handler = handler_status, .user_ctx = NULL };
static const httpd_uri_t uri_status_1 = { .uri = "/relay/1/status", .method = HTTP_GET, .handler = handler_status, .user_ctx = NULL };
static const httpd_uri_t uri_status_2 = { .uri = "/relay/2/status", .method = HTTP_GET, .handler = handler_status, .user_ctx = NULL };
static const httpd_uri_t uri_status_3 = { .uri = "/relay/3/status", .method = HTTP_GET, .handler = handler_status, .user_ctx = NULL };

static const httpd_uri_t uri_on_0 = { .uri = "/relay/0/on", .method = HTTP_GET, .handler = handler_on, .user_ctx = NULL };
static const httpd_uri_t uri_on_1 = { .uri = "/relay/1/on", .method = HTTP_GET, .handler = handler_on, .user_ctx = NULL };
static const httpd_uri_t uri_on_2 = { .uri = "/relay/2/on", .method = HTTP_GET, .handler = handler_on, .user_ctx = NULL };
static const httpd_uri_t uri_on_3 = { .uri = "/relay/3/on", .method = HTTP_GET, .handler = handler_on, .user_ctx = NULL };

static const httpd_uri_t uri_off_0 = { .uri = "/relay/0/off", .method = HTTP_GET, .handler = handler_off, .user_ctx = NULL };
static const httpd_uri_t uri_off_1 = { .uri = "/relay/1/off", .method = HTTP_GET, .handler = handler_off, .user_ctx = NULL };
static const httpd_uri_t uri_off_2 = { .uri = "/relay/2/off", .method = HTTP_GET, .handler = handler_off, .user_ctx = NULL };
static const httpd_uri_t uri_off_3 = { .uri = "/relay/3/off", .method = HTTP_GET, .handler = handler_off, .user_ctx = NULL };

static const httpd_uri_t uri_status_all = { .uri = "/relay/all/status", .method = HTTP_GET, .handler = handler_status, .user_ctx = NULL };
static const httpd_uri_t uri_on_all = { .uri = "/relay/all/on", .method = HTTP_GET, .handler = handler_on, .user_ctx = NULL };
static const httpd_uri_t uri_off_all = { .uri = "/relay/all/off", .method = HTTP_GET, .handler = handler_off, .user_ctx = NULL };

/*============================================================================
 * Public Functions
 *============================================================================*/

esp_err_t http_controller_init(void)
{
    ESP_LOGI(TAG, "Initializing HTTP server...");
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    // Apply configuration from config.h
    config.max_open_sockets = HTTP_MAX_CONNECTIONS;
    config.task_priority = HTTP_TASK_PRIORITY;
    config.stack_size = HTTP_TASK_STACK_SIZE;
    config.max_uri_handlers = 20;
    
#if HTTP_KEEP_ALIVE
    config.lru_purge_enable = true;
    config.keep_alive_enable = true;
    config.keep_alive_idle = 5;
    config.keep_alive_interval = 5;
    config.keep_alive_count = 3;
#endif
    
    ESP_LOGI(TAG, "Config: max_conn=%d, priority=%d, stack=%d, keep_alive=%d",
             HTTP_MAX_CONNECTIONS, HTTP_TASK_PRIORITY, 
             HTTP_TASK_STACK_SIZE, HTTP_KEEP_ALIVE);
    
    esp_err_t ret = httpd_start(&s_server, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start server: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Register URI handlers
    httpd_register_uri_handler(s_server, &uri_home);
    
    // Toggle endpoints
    httpd_register_uri_handler(s_server, &uri_toggle_0);
    httpd_register_uri_handler(s_server, &uri_toggle_1);
    httpd_register_uri_handler(s_server, &uri_toggle_2);
    httpd_register_uri_handler(s_server, &uri_toggle_3);
    
    // Status endpoints
    httpd_register_uri_handler(s_server, &uri_status_0);
    httpd_register_uri_handler(s_server, &uri_status_1);
    httpd_register_uri_handler(s_server, &uri_status_2);
    httpd_register_uri_handler(s_server, &uri_status_3);
    httpd_register_uri_handler(s_server, &uri_status_all);
    
    // On endpoints
    httpd_register_uri_handler(s_server, &uri_on_0);
    httpd_register_uri_handler(s_server, &uri_on_1);
    httpd_register_uri_handler(s_server, &uri_on_2);
    httpd_register_uri_handler(s_server, &uri_on_3);
    httpd_register_uri_handler(s_server, &uri_on_all);
    
    // Off endpoints
    httpd_register_uri_handler(s_server, &uri_off_0);
    httpd_register_uri_handler(s_server, &uri_off_1);
    httpd_register_uri_handler(s_server, &uri_off_2);
    httpd_register_uri_handler(s_server, &uri_off_3);
    httpd_register_uri_handler(s_server, &uri_off_all);
    
    ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);
    ESP_LOGI(TAG, "Endpoints registered:");
    ESP_LOGI(TAG, "  GET /                    - Home page UI");
    ESP_LOGI(TAG, "  GET /relay/{0-3}/toggle  - Toggle relay");
    ESP_LOGI(TAG, "  GET /relay/{0-3}/status  - Get status");
    ESP_LOGI(TAG, "  GET /relay/{0-3}/on      - Turn ON");
    ESP_LOGI(TAG, "  GET /relay/{0-3}/off     - Turn OFF");
    ESP_LOGI(TAG, "  GET /relay/all/status    - All statuses");
    ESP_LOGI(TAG, "  GET /relay/all/on        - All ON");
    ESP_LOGI(TAG, "  GET /relay/all/off       - All OFF");
    
    return ESP_OK;
}

esp_err_t http_controller_stop(void)
{
    if (s_server == NULL) {
        return ESP_OK;
    }
    
    esp_err_t ret = httpd_stop(s_server);
    s_server = NULL;
    
    ESP_LOGI(TAG, "HTTP server stopped");
    return ret;
}

httpd_handle_t http_controller_get_handle(void)
{
    return s_server;
}
