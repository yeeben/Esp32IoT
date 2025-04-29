#include "esp_err.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "tcp_client.h"
#include "wifi_sta_mode.h"
#include "payload_message.h"

#define HOST_IP_ADDRESS_SIZE 16
#define CONNECTED_WIFI_DELAY_S  10 * 1000
#define TRANSMIT_DATA_FREQ 2 * 1000

static const char *TAG = "wifi station";
static uint32_t sequence_id = 0;

void tcp_client_task(void *pvParameters)
{
    static char host_ip[HOST_IP_ADDRESS_SIZE] = "";
    char rx_buffer[sizeof(message_t)];
    int addr_family = 0;
    int ip_protocol = 0;
    message_t message = {0};

    while (1) {
        struct sockaddr_in dest_addr;
        while(!wifi_is_station_connected()) {
            vTaskDelay(CONNECTED_WIFI_DELAY_S / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "TCP Wifi Connected.");

        if(ESP_OK != wifi_get_host_ip_address(host_ip)) {
            ESP_LOGE(TAG, "TCP Host IP was not fetched");
            continue;
        }

        if(strlen(host_ip) < 1) {
            ESP_LOGE(TAG, "TCP Host IP was found to be empty");
            continue;

        }
        ESP_LOGI(TAG, "TCP Host GW IP %s", host_ip);

        int ret = inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        if (ret <= 0) {
            ESP_LOGE(TAG, "inet_pton failed: errno %d", errno);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(TCP_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        
        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            continue;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, TCP_PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);

            continue;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            if(ESP_OK == payload_message_load_mpu_sample(&message)) {
                message.sequence_id = sequence_id;
                err = send(sock, &message, sizeof(message_t), 0);
                if(err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
                sequence_id++;
            }
            payload_message_load_hello(&message);
            err = send(sock, &message, sizeof(message_t), 0);

            if(err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
            

            err = send(sock, &message, sizeof(message_t), 0);

        }
        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }

    }
}