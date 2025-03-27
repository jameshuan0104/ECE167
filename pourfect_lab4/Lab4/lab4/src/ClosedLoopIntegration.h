#ifndef CLOSED_LOOP_INTEGRATION_H
#define CLOSED_LOOP_INTEGRATION_H

#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//2 point calibration for accelerometer
#define ACC_X_FACEFOWARD -1000
#define ACC_X_FACEBACKWARD 980

#define ACC_Y_FACERIGHT -1050
#define ACC_Y_FACELEFT 960

#define ACC_Z_FACEUP 980
#define ACC_Z_FACEDOWN -1000

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
#define GYRO_SCALE_X 13500
#define GYRO_SCALE_Y 13500
#define GYRO_SCALE_Z 13500

//gyro bias, from averaging 10 mins of raw data (drift)
#define GYRO_BIAS_X -14
#define GYRO_BIAS_Y -17.5
#define GYRO_BIAS_Z 5.7

// Define constants
#define Kp_a 5.0f
#define Ki_a (Kp_a / 10.0f)
#define Kp_m 5.0f
#define Ki_m (Kp_m / 10.0f)

// Define a struct to hold Euler angles
typedef struct {
    float x, y, z;
} Vector3;

extern volatile int32_t x_avg_acc, y_avg_acc, z_avg_acc;

extern volatile int32_t x_avg_mag, y_avg_mag, z_avg_mag;

extern volatile int32_t x_avg_gyro, y_avg_gyro, z_avg_gyro;

extern volatile float angle_x, angle_y, angle_z;

// In ClosedLoopIntegration.h







void IntegrateClosedLoop(Vector3 gyros, Vector3 accels, Vector3 mags, Vector3 accelInertial, Vector3 magInertial, float deltaT, float* yaw, float* pitch, float* roll);

Vector3 DegreesToRadians(Vector3 degrees);

void collect_and_average_accelerometer(uint16_t num_samples);

void collect_and_average_magnetometer(uint16_t num_samples);

void collect_and_convert_gyroscope();

#endif // CLOSED_LOOP_INTEGRATION_H 
