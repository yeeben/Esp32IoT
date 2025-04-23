#ifndef WIFI_STA_MODE_H
#define WIFI_STA_MODE_H

#include "esp_err.h"

esp_err_t wifi_init_sta(const char* ssid, const char* password);
esp_err_t wifi_switch_to_sta(const char* ssid, const char* password);
bool wifi_is_station_connected();
esp_err_t wifi_get_host_ip_address(char *host_ip);

#endif