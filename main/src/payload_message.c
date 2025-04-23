#include <string.h>
#include "payload_message.h"


void payload_message_hello_message(message_t *message) {
    message->type = MSG_TYPE_HELLO;
    strncpy((char *)message->data, "I am a HELLO", sizeof(message->data) - 1);
    message->data[sizeof(message->data) - 1] = '\0';
    message->length = strlen((char *)message->data);
}

void payload_message_ssl(message_t *message) {
    message->type = MSG_TYPE_SSL;


}