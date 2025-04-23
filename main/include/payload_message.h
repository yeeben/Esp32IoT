#ifndef PAYLOAD_MESSAGE_H
#define PAYLOAD_MESSAGE_H
#include <stdint.h>

#define MAX_MESSAGE_SIZE 128

typedef enum {
    MSG_TYPE_HELLO = 0,
    MSG_TYPE_PAYLOAD = 1,
    MSG_TYPE_ACK = 2,
    MSG_TYPE_ERROR = 3
} message_type_t;

typedef struct {
    message_type_t type;
    uint16_t length;
    uint16_t sequence_id;
    char data[MAX_MESSAGE_SIZE];
} message_t;

#endif