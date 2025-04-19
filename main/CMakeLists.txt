idf_component_register(SRCS "main.c" "src/wifi_ap_config.c" "src/http_server.c"
                    PRIV_REQUIRES spi_flash esp_wifi nvs_flash esp_http_server
                    INCLUDE_DIRS "include")
