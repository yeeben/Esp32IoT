idf_component_register(SRCS "main.c" "src/wifi_ap_config.c"
                    PRIV_REQUIRES spi_flash esp_wifi nvs_flash
                    INCLUDE_DIRS "include")
