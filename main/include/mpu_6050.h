#ifndef MPU_6050_H
#define MPU_6050_H

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    
} mpu_accelerometer_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} mpu_gyro_t;

typedef struct {
    mpu_accelerometer_t acceleometer;
    uint16_t temperature;
    mpu_gyro_t gyro;
} mpu_sample_t;


void mpu_6050_sample(mpu_sample_t *sample);
void mpu_6050_display_sample(mpu_sample_t sample);
void mpu_6050_sampling_task();
#endif