#include "driver/i2c_master.h"

#define I2C_SDA_GPIO_PIN 21
#define I2C_SCL_GPIO_PIN 22
#define I2C_READ_TIMEOUT 1000

static i2c_master_bus_handle_t bus_handle;


void i2c_bus_init() {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = 0,
        .sda_io_num = I2C_SDA_GPIO_PIN,
        .scl_io_num = I2C_SCL_GPIO_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

}

void i2c_device_init(i2c_device_config_t *device_config, i2c_master_dev_handle_t *dev_handle) 
{
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, device_config, dev_handle));
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, device_config->device_address, I2C_READ_TIMEOUT));
}

void i2c_free_device(i2c_master_dev_handle_t *dev_handle) {
    i2c_master_bus_rm_device(*dev_handle);
}

void i2c_free_bus() {
    i2c_del_master_bus(bus_handle);
}

