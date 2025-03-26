#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <BNO055.h>
#include <timers.h>

//2 point calibration for accelerometer
#define ACC_X_FACEFOWARD -1006
#define ACC_X_FACEBACKWARD 990

#define ACC_Y_FACERIGHT -1070
#define ACC_Y_FACELEFT 920

#define ACC_Z_FACEUP 980
#define ACC_Z_FACEDOWN -1017

//expected values for magnetometer (raw values should be mapped to this)
#define EXPECTED_MAG_UP 41237.2
#define EXPECTED_MAG_DOWN -41237.2

#define EXPECTED_MAG_NORTH 23233.9
#define EXPECTED_MAG_SOUTH -23233.9

//2 point calibration for the magnetometer
#define MAG_X_NORTH -513
#define MAG_X_SOUTH 800

#define MAG_Y_NORTH -200
#define MAG_Y_SOUTH 910

#define MAG_Z_FACEUP -660
#define MAG_Z_FACEDOWN 460

//gyro scale factor clibration for each axis (raw sensor reading from turning gyro)
#define GYRO_SCALE_X 17000
#define GYRO_SCALE_Y 17000
#define GYRO_SCALE_Z 17000

//gyro bias, from averaging 10 mins of raw data (drift)
#define GYRO_BIAS_X -14
#define GYRO_BIAS_Y -17.5
#define GYRO_BIAS_Z 5.7

//global variables to store sensor values
static volatile int32_t x_avg_acc, y_avg_acc, z_avg_acc;

static volatile int32_t x_avg_mag, y_avg_mag, z_avg_mag;

static volatile int32_t x_avg_gyro, y_avg_gyro, z_avg_gyro;

static volatile float angle_x = 0.0f, angle_y = 0.0f, angle_z = 0.0f;

// Collects 'num_samples' and returns the average for accelerometer
void collect_and_average_accelerometer(uint16_t num_samples) {
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    int32_t z_sum = 0;
    for (uint16_t i = 0; i < num_samples; i++) {
        // Read sensor and accumulate sum
        x_sum += BNO055_ReadAccelX();
        y_sum += BNO055_ReadAccelY();
        z_sum += BNO055_ReadAccelZ();
        //printf("%d, %d, %d\n", x_sum, y_sum, z_sum);
    }
    x_avg_acc = x_sum / num_samples; // Return average
    y_avg_acc = y_sum / num_samples;
    z_avg_acc = z_sum / num_samples;
}

// Collects 'num_samples' and returns the average for magnetometer
void collect_and_average_magnetometer(uint16_t num_samples) {
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    int32_t z_sum = 0;
    for (uint16_t i = 0; i < num_samples; i++) {
        // Read sensor and accumulate sum
        x_sum += BNO055_ReadMagX();
        y_sum += BNO055_ReadMagY();
        z_sum += BNO055_ReadMagZ();
        //printf("%d, %d, %d\n", x_sum, y_sum, z_sum);
    }
    x_avg_mag = x_sum / num_samples; // Return average
    y_avg_mag = y_sum / num_samples;
    z_avg_mag = z_sum / num_samples;
}

//collect raw gyro data and converted to degree
void collect_and_convert_gyroscope() {
    float dt = 0.02;

    float gyro_raw_x = 0;
    float gyro_raw_y = 0;
    float gyro_raw_z = 0;
    
    gyro_raw_x += BNO055_ReadGyroX();
    gyro_raw_y += BNO055_ReadGyroY();    
    gyro_raw_z += BNO055_ReadGyroZ();
    //printf("%d, %d, %d\n", gyro_raw_x, gyro_raw_y, gyro_raw_z);

    // Convert to °/s and integrate
    angle_x += ((gyro_raw_x - GYRO_BIAS_X) / GYRO_SCALE_X) * dt * 180;
    angle_y += ((gyro_raw_y - GYRO_BIAS_Y) / GYRO_SCALE_Y) * dt * 180;
    angle_z += ((gyro_raw_z - GYRO_BIAS_Z) / GYRO_SCALE_Z) * dt * 180;
}



int main(void) {
    //init all hardware
    BOARD_Init();
    BNO055_Init();
    TIMER_Init();
    while(1){
        //get raw sensor readings
        collect_and_average_accelerometer(1);

        //2 point calibration for accelerometer, calcualting bias and scale factor
        float x_scale_factor = (ACC_X_FACEBACKWARD-ACC_X_FACEFOWARD) / -2;
        float x_bias = (ACC_X_FACEFOWARD+(ACC_X_FACEBACKWARD)) / 2;
        float x_calibrated = (x_avg_acc - x_bias) / x_scale_factor;
        //printf("x: %.2f, %.2f\n", x_scale_factor, x_bias);

        float y_scale_factor = (ACC_Y_FACELEFT-ACC_Y_FACERIGHT) / -2;
        float y_bias = (ACC_Y_FACERIGHT+(ACC_Y_FACELEFT)) / 2;
        float y_calibrated = (y_avg_acc - y_bias) / y_scale_factor;
        //printf("y: %.2f, %.2f\n", y_scale_factor, y_bias);

        float z_scale_factor = (ACC_Z_FACEDOWN-ACC_Z_FACEUP) / -2;
        float z_bias = (ACC_Z_FACEUP+(ACC_Z_FACEDOWN)) / 2;
        float z_calibrated = (z_avg_acc - z_bias) / z_scale_factor;
        //printf("z: %.2f, %.2f\n", z_scale_factor, z_bias);
        
        //printf("%.2f, %.2f, %.2f\n", x_calibrated, y_calibrated, z_calibrated);

        //get raw sensor readings
        collect_and_average_magnetometer(1);

        //2 point calibrations for the magnetometer
        float x_scale_factor_mag = (EXPECTED_MAG_SOUTH-EXPECTED_MAG_NORTH) / (MAG_X_SOUTH-MAG_X_NORTH);
        float x_bias_mag = EXPECTED_MAG_NORTH - (x_scale_factor_mag*MAG_X_NORTH);
        float x_calibrated_mag = (x_scale_factor_mag*x_avg_mag) + x_bias_mag;
        //printf("x: %.2f, %.2f\n", x_scale_factor_mag, x_bias_mag);

        float y_scale_factor_mag = (EXPECTED_MAG_SOUTH-EXPECTED_MAG_NORTH) / (MAG_Y_SOUTH-MAG_Y_NORTH);
        float y_bias_mag = EXPECTED_MAG_NORTH - (y_scale_factor_mag*MAG_Y_NORTH);
        float y_calibrated_mag = (y_scale_factor_mag*y_avg_mag) + y_bias_mag;
        //printf("y: %.2f, %.2f\n", y_scale_factor_mag, y_bias_mag);

        float z_scale_factor_mag = (EXPECTED_MAG_DOWN-EXPECTED_MAG_UP) / (MAG_Z_FACEDOWN-MAG_Z_FACEUP);
        float z_bias_mag = EXPECTED_MAG_UP - (z_scale_factor_mag*MAG_Z_FACEUP);
        float z_calibrated_mag = (z_scale_factor_mag*z_avg_mag) + z_bias_mag;
        //printf("z: %.2f, %.2f\n", z_scale_factor_mag, z_bias_mag);
        
        //printf("\r%.2f, %.2f, %.2f", x_calibrated_mag, y_calibrated_mag, z_calibrated_mag);

        //collect and calibrate gyro, all previous code may need to be uncommented due to timing
        collect_and_convert_gyroscope();
        HAL_Delay(20); //delay to 50Hz or 20ms
        //printf("%.2f, %.2f, %.2f\n", angle_x, angle_y, angle_z);
    }
}
