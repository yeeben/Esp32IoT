#ifndef PAYLOAD_MESSAGE_H
#define PAYLOAD_MESSAGE_H
#include <stdint.h>
#include "esp_err.h"

#define MAX_MESSAGE_SIZE 256

typedef enum {
    MSG_TYPE_BASE,
    MSG_TYPE_MPU_DATA,
    MSG_TYPE_HELLO,
} message_type_t;

typedef struct {
    message_type_t type;
    uint16_t length;
    uint32_t sequence_id;
    uint8_t data[MAX_MESSAGE_SIZE];
} message_t;

esp_err_t payload_message_load_mpu_sample(message_t *message);
esp_err_t payload_message_load_hello(message_t *message);
#endif