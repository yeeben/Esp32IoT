#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "http_server.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* Handler function for the WiFi provisioning page */
static esp_err_t wifi_provision_handler(httpd_req_t *req)
{
    const char* html = "<!DOCTYPE html><html><head>"
        "<title>WiFi Setup</title>"
        "<style>"
        "body{font-family:Arial,sans-serif;margin:40px auto;max-width:650px;line-height:1.6;padding:0 10px}"
        "input{width:100%;padding:8px;margin:8px 0;box-sizing:border-box}"
        "button{background:#4CAF50;color:white;padding:10px 15px;border:none;cursor:pointer}"
        "</style></head>"
        "<body>"
        "<h2>WiFi Configuration</h2>"
        "<form method='POST' action='/configure_wifi'>"
        "<label for='ssid'>WiFi Network Name (SSID):</label><br>"
        "<input type='text' id='ssid' name='ssid' required><br>"
        "<label for='password'>WiFi Password:</label><br>"
        "<input type='password' id='password' name='password' required><br><br>"
        "<button type='submit'>Connect</button>"
        "</form></body></html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, strlen(html));
    return ESP_OK;
}

/* Handler for processing the submitted WiFi credentials */
static esp_err_t wifi_configure_handler(httpd_req_t *req)
{
    char content[256];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[recv_size] = '\0';

    char ssid[33] = {0};    // Max SSID length is 32 bytes
    char password[65] = {0}; // Max WPA2 password length is 64 bytes
    
    // Parse POST data (simple implementation)
    char *ssid_start = strstr(content, "ssid=");
    char *pass_start = strstr(content, "password=");
    
    if (ssid_start && pass_start) {
        ssid_start += 5;  // Skip "ssid="
        pass_start += 9;  // Skip "password="
        
        char *ssid_end = strchr(ssid_start, '&');
        if (ssid_end) {
            memcpy(ssid, ssid_start, MIN(32, ssid_end - ssid_start));
        }
        
        memcpy(password, pass_start, MIN(64, strlen(pass_start)));
        
        // TODO: Add your WiFi connection logic here using ssid and password
        
        const char* response = "WiFi credentials received. Attempting to connect...";
        httpd_resp_send(req, response, strlen(response));
        return ESP_OK;
    }

    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request");
    return ESP_FAIL;
}

/* URI handlers for WiFi provisioning */
httpd_uri_t wifi_provision_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = wifi_provision_handler,
    .user_ctx = NULL
};

httpd_uri_t wifi_configure_post = {
    .uri      = "/configure_wifi",
    .method   = HTTP_POST,
    .handler  = wifi_configure_handler,
    .user_ctx = NULL
};


/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &wifi_provision_get);
        httpd_register_uri_handler(server, &wifi_configure_post);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}