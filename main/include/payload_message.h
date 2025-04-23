#ifndef PAYLOAD_MESSAGE_H
#define PAYLOAD_MESSAGE_H
#include <stdint.h>

#define MAX_MESSAGE_SIZE 256

typedef enum {
    MSG_TYPE_HELLO,
    MSG_TYPE_SSL,
    MSG_TYPE_ACK,
    MSG_TYPE_ERROR,
    MSG_TYPE_MAX,
} message_type_t;

typedef struct {
    message_type_t type;
    uint16_t length;
    uint16_t sequence_id;
    uint8_t data[MAX_MESSAGE_SIZE];
} message_t;

#endif