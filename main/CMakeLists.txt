idf_component_register(SRCS "main.c" "src/wifi_ap_config.c" 
                            "src/http_server.c" 
                            "src/wifi_sta_mode.c"
                            "src/sensor_data.c"
                            "src/tcp_client.c"
                            "src/payload_message.c"
                    PRIV_REQUIRES spi_flash esp_wifi nvs_flash esp_http_server
                    INCLUDE_DIRS "include")
