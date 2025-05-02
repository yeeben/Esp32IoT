idf_component_register(SRCS "main.c" "src/wifi_ap_config.c" 
                            "src/http_server.c" 
                            "src/wifi_sta_mode.c"
                            "src/tcp_client.c"
                            "src/payload_message.c"
                            "src/i2c_driver.c"
                            "src/mpu_6050.c"
                    PRIV_REQUIRES spi_flash esp_wifi nvs_flash esp_http_server esp_driver_i2c esp_timer
                    INCLUDE_DIRS "include")
