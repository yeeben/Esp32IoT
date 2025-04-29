#include <string.h>
#include "payload_message.h"
#include "mpu_6050.h"
#include "freertos/FreeRTOS.h"

esp_err_t payload_message_load_mpu_sample(message_t *message) {
    mpu_sample_t sample = {0};
    BaseType_t result = pdFALSE;

    if(!is_mpu_queue_init()) {
        return ESP_ERR_INVALID_STATE;
    }
    result = mpu_queue_receive(&sample);
    if(result != pdTRUE) {
        return ESP_ERR_NOT_FOUND;
    }

    message->type = MSG_TYPE_MPU_DATA;
    message->length = sizeof(mpu_sample_t);
    memcpy(message->data, &sample, sizeof(mpu_sample_t));

    return ESP_OK;
}

esp_err_t payload_message_load_hello(message_t *message) {
    message->type = MSG_TYPE_HELLO;
    message->length = 0;
    memset(message->data, 0, sizeof(message->data));
    return ESP_OK;
}
