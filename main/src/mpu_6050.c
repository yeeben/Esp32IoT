#include "esp_log.h"
#include "esp_err.h"
#include "string.h"
#include "freertos/FreeRTOS.h"

#include "i2c_driver.h"
#include "mpu_6050.h"
#define MPU_6050_ADDRESS 0x68
#define I2C_CLK_SPEED_HZ (100 * 1000)
#define I2C_TRANSACTION_TIMEOUT 500
#define MPU_6050_SAMPLING_RATE 1 //Hz
#define RTOS_SAMPLING_DELAY_MS 50

#define MPU_SELF_TEST_X_ADDRESS      0x0D
#define MPU_SELF_TEST_Y_ADDRESS      0x0E
#define MPU_SELF_TEST_Z_ADDRESS      0x0F
#define MPU_SELF_TEST_A_ADDRESS      0x10
#define MPU_ACCEL_XOUT_H_ADDRESS     0x3B
#define MPU_ACCEL_XOUT_L_ADDRESS     0x3C
#define MPU_ACCEL_YOUT_H_ADDRESS     0x3D
#define MPU_ACCEL_YOUT_L_ADDRESS     0x3E
#define MPU_ACCEL_ZOUT_H_ADDRESS     0x3F
#define MPU_ACCEL_ZOUT_L_ADDRESS     0x40
#define MPU_TEMP_OUT_H_ADDRESS       0x41
#define MPU_TEMP_OUT_L_ADDRESS       0x42
#define MPU_GYRO_XOUT_H_ADDRESS      0x43
#define MPU_GYRO_XOUT_L_ADDRESS      0x44
#define MPU_GYRO_YOUT_H_ADDRESS      0x45
#define MPU_GYRO_YOUT_L_ADDRESS      0x46
#define MPU_GYRO_ZOUT_H_ADDRESS      0x47
#define MPU_GYRO_ZOUT_L_ADDRESS      0x48
#define MPU9250_PWR_MGMT_1_REG_ADDRESS 0x6B
#define MPU_WHO_AM_I_ADDRESS 0x75

#define MPU9250_RESET_BIT           7

static QueueHandle_t xMPUQueue;
static i2c_master_dev_handle_t dev_handle;
static uint8_t read_mpu[2] = {0};
static const char *TAG = "MPU_6050";
static bool queue_is_initalized = false;

bool is_mpu_queue_init() {
    return queue_is_initalized;
}

static esp_err_t mpu_6050_register_read(uint8_t reg, uint8_t *data, size_t len) {
    esp_err_t result = ESP_ERR_NOT_FINISHED;
    result = i2c_master_transmit_receive(dev_handle, &reg, 1, data, len, I2C_TRANSACTION_TIMEOUT);
    return result;

}

static esp_err_t mpu_6050_register_write(uint8_t reg, uint8_t data) {
    esp_err_t result = ESP_ERR_NOT_FINISHED;
    uint8_t write_buf[2] = {reg, data};

    result = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_TRANSACTION_TIMEOUT);
    return result;
}
void mpu_6050_init()
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MPU_6050_ADDRESS,
        .scl_speed_hz = I2C_CLK_SPEED_HZ,
    };
    i2c_device_init(&dev_config, &dev_handle);

    mpu_6050_register_write(MPU9250_PWR_MGMT_1_REG_ADDRESS, 0);
}

void mpu_queue_init() {
    xMPUQueue = xQueueCreate( 40, sizeof( mpu_sample_t ) );
    queue_is_initalized = true;

}

BaseType_t mpu_queue_send(mpu_sample_t sample) {
    BaseType_t result = pdFALSE;
    result = xQueueSend(xMPUQueue, &sample, 500);
    if( result != pdTRUE) {
        ESP_LOGI(TAG, "Queue full\n");
        return result;
    }
    return result;
}

BaseType_t mpu_queue_receive(mpu_sample_t *sample) {
    BaseType_t result = pdFALSE;
    result = xQueueReceive(xMPUQueue, sample, 500);
    if (result != pdTRUE) {
        ESP_LOGE(TAG, "Did not received data in the past %d ms", 500);
    }
    return result;
}


void mpu_6050_who_am_i() {
    ESP_ERROR_CHECK(mpu_6050_register_read(MPU_WHO_AM_I_ADDRESS, read_mpu, 1));
    ESP_LOGI(TAG, "WHO_AM_I = %X", read_mpu[0]);
}  

void mpu_6050_self_test() {
    ESP_ERROR_CHECK(mpu_6050_register_read(MPU_SELF_TEST_X_ADDRESS, read_mpu, 1));
    ESP_LOGI(TAG, "MPU_SELF_TEST_X = %X", read_mpu[0]);

    ESP_ERROR_CHECK(mpu_6050_register_read(MPU_SELF_TEST_Y_ADDRESS, read_mpu, 1));
    ESP_LOGI(TAG, "MPU_SELF_TEST_Y = %X", read_mpu[0]);

    ESP_ERROR_CHECK(mpu_6050_register_read(MPU_SELF_TEST_Z_ADDRESS, read_mpu, 1));
    ESP_LOGI(TAG, "MPU_SELF_TEST_Z = %X", read_mpu[0]);

    ESP_ERROR_CHECK(mpu_6050_register_read(MPU_SELF_TEST_A_ADDRESS, read_mpu, 1));
    ESP_LOGI(TAG, "MPU_SELF_TEST_A = %X", read_mpu[0]);
}

void mpu_6050_display_sample(mpu_sample_t sample) {
    ESP_LOGI(TAG, "accel: %d %d %d", sample.acceleometer.x, sample.acceleometer.y, sample.acceleometer.z);
    ESP_LOGI(TAG, "Gyro: %d %d %d", sample.gyro.x, sample.gyro.y, sample.gyro.z);
}


void mpu_6050_sample_accel(mpu_accelerometer_t *accel) {

    // Acccelerometer Reads
    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_XOUT_H_ADDRESS, &read_mpu[1], 1));
    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_XOUT_L_ADDRESS, &read_mpu[0], 1));
    accel->x = (read_mpu[1] << 8) | read_mpu[0];

    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_YOUT_H_ADDRESS, &read_mpu[1], 1));
    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_YOUT_L_ADDRESS, &read_mpu[0], 1));
    accel->y = (read_mpu[1] << 8) | read_mpu[0];

    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_ZOUT_H_ADDRESS, &read_mpu[1], 1));
    ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_ACCEL_ZOUT_L_ADDRESS, &read_mpu[0], 1));
    accel->z = (read_mpu[1] << 8) | read_mpu[0];


}

void mpu_6050_sample_gyro(mpu_gyro_t *gyro) {
        // Gyro Reads
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_XOUT_H_ADDRESS, &read_mpu[1], 1));
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_XOUT_L_ADDRESS, &read_mpu[0], 1));
        gyro->x = (read_mpu[1] << 8) | read_mpu[0];
    
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_YOUT_H_ADDRESS, &read_mpu[1], 1));
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_YOUT_L_ADDRESS, &read_mpu[0], 1));
        gyro->y = (read_mpu[1] << 8) | read_mpu[0];
    
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_ZOUT_H_ADDRESS, &read_mpu[1], 1));
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_GYRO_ZOUT_L_ADDRESS, &read_mpu[0], 1));
        gyro->z = (read_mpu[1] << 8) | read_mpu[0];
}    

void mpu_6050_sample_temp(uint16_t *temp) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_TEMP_OUT_H_ADDRESS, &read_mpu[1], 1));
        ESP_ERROR_CHECK_WITHOUT_ABORT(mpu_6050_register_read(MPU_TEMP_OUT_L_ADDRESS, &read_mpu[0], 1));
        *temp = (read_mpu[1] << 8) | read_mpu[0];
    
}

void mpu_6050_sampling_task() {
    mpu_6050_init();
    mpu_queue_init();
    mpu_6050_who_am_i();
    mpu_6050_self_test();
    mpu_sample_t sample = {0};
    ESP_LOGI(TAG, "Sample Task Started");


    while(1) {
        mpu_6050_sample_accel(&(sample.acceleometer));
        mpu_6050_sample_gyro(&(sample.gyro));
        mpu_6050_sample_temp(&(sample.temperature));
        mpu_queue_send(sample);
        vTaskDelay(RTOS_SAMPLING_DELAY_MS);
    }

}
