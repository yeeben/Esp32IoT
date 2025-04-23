#include <string.h>
#include "payload_message.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"


void payload_message_hello_message(message_t *message) {
    message->type = MSG_TYPE_HELLO;
    strncpy((char *)message->data, "I am a HELLO", sizeof(message->data) - 1);
    message->data[sizeof(message->data) - 1] = '\0';
    message->length = strlen((char *)message->data);
}

void payload_message_ssl(message_t *message) {
    message->type = MSG_TYPE_SSL;


}