#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "driver/i2c_master.h"
#include "esp_err.h"

void i2c_bus_init();
void i2c_device_init(i2c_device_config_t *device_config, i2c_master_dev_handle_t *dev_handle);
void i2c_free_device(i2c_master_dev_handle_t *dev_handle);

#endif