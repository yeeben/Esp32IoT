#include "esp_log.h"

#include "i2c_driver.h"
#include "mpu_6050.h"
#define MPU_6050_ADDRESS 0x68
#define I2C_CLK_SPEED_HZ (100 * 1000)
#define I2C_TRANSACTION_TIMEOUT 1000


#define WHO_AM_I_ADDRESS 0x75

static i2c_master_dev_handle_t dev_handle;
static uint8_t read_mpu;
static uint8_t write_mpu;

static const char *TAG = "MPU_6050";

void mpu_6050_init()
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MPU_6050_ADDRESS,
        .scl_speed_hz = I2C_CLK_SPEED_HZ,
    };
    i2c_device_init(&dev_config, &dev_handle);
    
}

void mpu_6050_who_am_i() {
    write_mpu = WHO_AM_I_ADDRESS;
    i2c_master_transmit_receive(dev_handle, &write_mpu, 1, &read_mpu, 1, I2C_TRANSACTION_TIMEOUT);
    ESP_LOGI(TAG, "WHO_AM_I = %X", read_mpu);
}   

