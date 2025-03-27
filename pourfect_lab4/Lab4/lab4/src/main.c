#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <BNO055.h>
#include "OpenLoopIntegration.h"
#include "ClosedLoopIntegration.h"
#include <Oled.h>
#include <timers.h>


#define OPEN_LOOP
#define CLOSED_LOOP

int main(){
    BOARD_Init();
    BNO055_Init();
    TIMER_Init();
    OledInit();
    float yaw = 0, pitch = 0, roll = 0;
    char OledString[50];
    float x_scale_factor = (ACC_X_FACEBACKWARD-ACC_X_FACEFOWARD) / -2;
    float x_bias = (ACC_X_FACEFOWARD+(ACC_X_FACEBACKWARD)) / 2;

    float y_scale_factor = (ACC_Y_FACELEFT-ACC_Y_FACERIGHT) / -2;
    float y_bias = (ACC_Y_FACERIGHT+(ACC_Y_FACELEFT)) / 2;
     
    float z_scale_factor = (ACC_Z_FACEDOWN-ACC_Z_FACEUP) / -2;
    float z_bias = (ACC_Z_FACEUP+(ACC_Z_FACEDOWN)) / 2;

    float x_scale_factor_mag = (EXPECTED_MAG_SOUTH-EXPECTED_MAG_NORTH) / (MAG_X_SOUTH-MAG_X_NORTH);
    float x_bias_mag = EXPECTED_MAG_NORTH - (x_scale_factor_mag*MAG_X_NORTH);
 
    float y_scale_factor_mag = (EXPECTED_MAG_SOUTH-EXPECTED_MAG_NORTH) / (MAG_Y_SOUTH-MAG_Y_NORTH);
    float y_bias_mag = EXPECTED_MAG_NORTH - (y_scale_factor_mag*MAG_Y_NORTH);

    float z_scale_factor_mag = (EXPECTED_MAG_DOWN-EXPECTED_MAG_UP) / (MAG_Z_FACEDOWN-MAG_Z_FACEUP);
    float z_bias_mag = EXPECTED_MAG_UP - (z_scale_factor_mag*MAG_Z_FACEUP);

    while(1){
         //2 point calibration for accelerometer, calcualting bias and scale factor
         collect_and_average_accelerometer(1);
         float x_calibrated = (x_avg_acc - x_bias) / x_scale_factor;
         //printf("x: %.2f, %.2f\n", x_scale_factor, x_bias);
         float y_calibrated = (y_avg_acc - y_bias) / y_scale_factor;
         //printf("y: %.2f, %.2f\n", y_scale_factor, y_bias);
         float z_calibrated = (z_avg_acc - z_bias) / z_scale_factor;
         //printf("z: %.2f, %.2f\n", z_scale_factor, z_bias);
         
         //printf("\rcalibrated: %.2f, %.2f, %.2f\n", x_calibrated, y_calibrated, z_calibrated);
 
         //get raw sensor readings
         collect_and_average_magnetometer(1);
 
         //2 point calibrations for the magnetometer
         float x_calibrated_mag = (x_scale_factor_mag*x_avg_mag) + x_bias_mag;
         //printf("x: %.2f, %.2f\n", x_scale_factor_mag, x_bias_mag);
         float y_calibrated_mag = (y_scale_factor_mag*y_avg_mag) + y_bias_mag;
         //printf("y: %.2f, %.2f\n", y_scale_factor_mag, y_bias_mag);
 
         float z_calibrated_mag = (z_scale_factor_mag*z_avg_mag) + z_bias_mag;
         //printf("z: %.2f, %.2f\n", z_scale_factor_mag, z_bias_mag);
         
         //printf("\r%.2f, %.2f, %.2f", x_calibrated_mag, y_calibrated_mag, z_calibrated_mag);
         
         //collect and calibrate gyro, all previous code may need to be uncommented due to timing
         collect_and_convert_gyroscope();



         //printf("\rdegree: X: %.2f°, Y: %.2f°, Z: %.2f°", angle_x, angle_y, angle_z);
         
         Vector3 gyros_deg = {angle_x, angle_y, angle_z}; // Gyroscope data (rad/s)
         //printf("Gyro: %.2f, %.2f, %.2f\n", angle_x, angle_y, angle_z);
         Vector3 gyros_rad = DegreesToRadians(gyros_deg);
         Vector3 accels = {x_calibrated, y_calibrated, z_calibrated};  // Accelerometer data (g)
         Vector3 accelInertial = {0.0f, 0.0f, -1.0f}; // Inertial gravity vector 
         Vector3 mags = {x_calibrated_mag, y_calibrated_mag, z_calibrated_mag};
         Vector3 magInertial = {-23000.0f, 1000.0f, -41000.0f};  // Magnetic field points towards magnetic north
         float deltaT = 0.02f; // Time step (s)
 
        // Integrate orientation
        IntegrateClosedLoop(gyros_rad, accels, mags, accelInertial, magInertial, deltaT, &yaw, &pitch, &roll);
       // printf("------Closed Loop-----/nYaw: %.2f, Pitch: %.2f, Roll: %.2f\n", yaw, pitch, roll);

        sprintf(OledString, "Yaw: %.2f\nPitch: %.2f\nRoll: %.2f\n", yaw, pitch, roll);

        OledDrawString(OledString);
        OledUpdate();

        #ifdef OPEN_LOOP
        float p = ((BNO055_ReadGyroX()) * M_PI / 180.0);   // covert Gyro of X into radians/sec
        float q = ((BNO055_ReadGyroY()) * M_PI / 180.0);   // covert Gyro of Y into radians/sec
        float r = ((BNO055_ReadGyroZ()) * M_PI / 180.0);  // covert Gyro of Z into radians/sec

        OpenLoopIntegrate(p,q,r, &yaw, &pitch, &roll);
        printf("\n------Open Loop-----\nYaw: %.2f, Pitch: %.2f, Roll: %.2f\n", yaw, pitch, roll);
        #endif



        HAL_Delay(20);
    }
}