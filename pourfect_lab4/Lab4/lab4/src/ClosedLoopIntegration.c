#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <BNO055.h>
#include <timers.h>
#include <math.h>
#include "ClosedLoopIntegration.h"
#include "Euler.h"
#include "MatrixMath.h"

// Define vector and matrix types
typedef struct {
    float m[3][3];
} Matrix3x3;


// Function to print a 3x3 matrix
void print_matrix(const char* name, Matrix3x3 mat) {
    printf("%s:\n", name);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%f\t", mat.m[i][j]);  // Print each element of the matrix
        }
        printf("\n");  // New line after each row
    }
}

// // Define a struct to hold Euler angles
typedef struct {
    float yaw;   // ψ (rotation around z-axis)
    float pitch; // θ (rotation around y-axis)
    float roll;  // φ (rotation around x-axis)
} EulerAngles;


// // Initialize rotation matrix and bias estimate
static float BiasMatrix[3][3] = {
    {0.9918,  0.1275, -0.0044},
    {-0.1274, 0.9879, -0.0885},
    {-0.0070, 0.0883,  0.9961}
};

static volatile Matrix3x3 R = {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}};
static volatile Vector3 biasEstimate = {0, 0, 0};

volatile float angle_x = 0.0f, angle_y = 0.0f, angle_z = 0.0f;
volatile int32_t x_avg_acc, y_avg_acc, z_avg_acc;

volatile int32_t x_avg_mag, y_avg_mag, z_avg_mag;

volatile int32_t x_avg_gyro, y_avg_gyro, z_avg_gyro;

//global variables to store sensor values


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
        //printf("raw: %d, %d, %d\n", x_sum, y_sum, z_sum);
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
    float magVector[3] = {x_avg_mag, y_avg_mag, z_avg_mag};
    float resVector[3] = {0, 0, 0};

    MatrixVectorMultiply(BiasMatrix, magVector, resVector); //Apply misalignment correction

    x_avg_mag = resVector[0];
    y_avg_mag = resVector[1];
    z_avg_mag = resVector[2];
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


    // Convert to °/s and integrate
    angle_x += ((gyro_raw_x - GYRO_BIAS_X) / GYRO_SCALE_X) * dt * 180;
    angle_y += ((gyro_raw_y - GYRO_BIAS_Y) / GYRO_SCALE_Y) * dt * 180;
    angle_z += ((gyro_raw_z - GYRO_BIAS_Z) / GYRO_SCALE_Z) * dt * 180;
}

// Helper function to compute cross product
Vector3 cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// Function to compute the norm of a vector
float vector_norm(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Function to normalize a vector
Vector3 normalize(Vector3 v) {
    float norm = vector_norm(v);  // Compute the norm of the vector
    if (norm != 0.0f) {           // Avoid division by zero
        v.x /= norm;              // Divide each component by the norm
        v.y /= norm;
        v.z /= norm;
    }
    return v;  // Return the normalized vector
}

// Helper function to compute matrix exponential (simplified for small angles)
Matrix3x3 Rexp(Vector3 w, float deltaT) {
    Matrix3x3 result = {{
        {1.0f, -w.z * deltaT, w.y * deltaT},
        {w.z * deltaT, 1.0f, -w.x * deltaT},
        {-w.y * deltaT, w.x * deltaT, 1.0f}
    }};
    return result;
}

// Function to extract Euler angles from a rotation matrix
EulerAngles ExtractEulerAngles(Matrix3x3 R) {
    EulerAngles angles;

    DCMtoEuler(R.m, &angles.yaw, &angles.pitch, &angles.roll);

    return angles;
}


// Main function to integrate gyroscope data with closed-loop correction (no magnetometer)
void IntegrateClosedLoop(Vector3 gyros, Vector3 accels, Vector3 mags, Vector3 accelInertial, Vector3 magInertial, float deltaT, float* yaw, float* pitch, float* roll) {
    // Normalize accelerometer reading and inertial reference vector
    accels = normalize(accels);
    mags = normalize(mags);
    accelInertial = normalize(accelInertial);
    magInertial = normalize(magInertial);

    // Remove bias from gyroscope readings
    Vector3 gyroInputWithBias = {gyros.x - biasEstimate.x, gyros.y - biasEstimate.y, gyros.z - biasEstimate.z};

    // Compute accelerometer feedback term using cross product
    Vector3 wmeas_a = cross(accels, (Vector3){
        R.m[0][0] * accelInertial.x + R.m[0][1] * accelInertial.y + R.m[0][2] * accelInertial.z,
        R.m[1][0] * accelInertial.x + R.m[1][1] * accelInertial.y + R.m[1][2] * accelInertial.z,
        R.m[2][0] * accelInertial.x + R.m[2][1] * accelInertial.y + R.m[2][2] * accelInertial.z
    });

    Vector3 wmeas_m = cross(mags, (Vector3){
        R.m[0][0] * magInertial.x + R.m[0][1] * magInertial.y + R.m[0][2] * magInertial.z,
        R.m[1][0] * magInertial.x + R.m[1][1] * magInertial.y + R.m[1][2] * magInertial.z,
        R.m[2][0] * magInertial.x + R.m[2][1] * magInertial.y + R.m[2][2] * magInertial.z
    });

    // Apply feedback to gyroscope input (only accelerometer feedback)
    Vector3 gyroInputWithFeedback = {
        gyroInputWithBias.x + Kp_a * wmeas_a.x + Kp_m * wmeas_m.x,
        gyroInputWithBias.y + Kp_a * wmeas_a.y + Kp_m * wmeas_m.y,
        gyroInputWithBias.z + Kp_a * wmeas_a.z + Kp_m * wmeas_m.z
    };

    // Update bias estimate (only accelerometer feedback)
    Vector3 bdot = {
        -Ki_a * wmeas_a.x - Ki_m * wmeas_m.x,
        -Ki_a * wmeas_a.y - Ki_m * wmeas_m.y,
        -Ki_a * wmeas_a.z - Ki_m * wmeas_m.z
    };
    biasEstimate.x += bdot.x * deltaT;
    biasEstimate.y += bdot.y * deltaT;
    biasEstimate.z += bdot.z * deltaT;

    //printf("\rYaw: %5.2f°, Pitch: %5.2f°, Roll: %5.2f°", gyroInputWithFeedback.x, gyroInputWithFeedback.y, gyroInputWithFeedback.z);
    Vector3 Bplus = {
        biasEstimate.x + bdot.x * deltaT,
        biasEstimate.y + bdot.y * deltaT,
        biasEstimate.z + bdot.z * deltaT
    };
    biasEstimate = Bplus;

    // Update rotation matrix using matrix exponential
    Matrix3x3 deltaR = Rexp(gyroInputWithFeedback, deltaT);
    //deltaR has non-zero output as i turn sensor
    // Compute Rplus = deltaR * R
    Matrix3x3 Rplus = {{{0}}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Rplus.m[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                Rplus.m[i][j] += deltaR.m[i][k] * R.m[k][j];
            }
        }
    }
    //print_matrix("Rplus", Rplus); //this line Rplus is all zeros
    R = Rplus;
    
    // Print the updated rotation matrix (Rplus)
    //print_matrix("Updated R (Rplus)", Rplus);

    // Extract Euler angles from the rotation matrix
    EulerAngles angles = ExtractEulerAngles(Rplus); 
    *yaw = angles.yaw;
    *pitch = angles.pitch;
    *roll = angles.roll;
}

// Convert degrees to radians
Vector3 DegreesToRadians(Vector3 degrees) {
    Vector3 radians;
    radians.x = degrees.x * (M_PI / 180.0f);
    radians.y = degrees.y * (M_PI / 180.0f);
    radians.z = degrees.z * (M_PI / 180.0f);
    return radians;
}
#ifdef TEST
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
        
        //printf("\rcalibrated: %.2f, %.2f, %.2f\n", x_calibrated, y_calibrated, z_calibrated);

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
        //printf("\rdegree: X: %.2f°, Y: %.2f°, Z: %.2f°", angle_x, angle_y, angle_z);
        
        // Example sensor data
        Vector3 gyros_deg = {angle_x, angle_y, angle_z}; // Gyroscope data (rad/s)
        Vector3 gyros_rad = DegreesToRadians(gyros_deg);
        Vector3 accels = {x_calibrated, y_calibrated, z_calibrated};  // Accelerometer data (g)
        Vector3 accelInertial = {0.0f, 0.0f, -1.0f}; // Inertial gravity vector 
        Vector3 mags = {x_calibrated_mag, y_calibrated_mag, z_calibrated_mag};
        Vector3 magInertial = {1.0f, 0.0f, 0.0f};  // Magnetic field points towards magnetic north
        float deltaT = 0.02f; // Time step (s)

        // Integrate orientation
        IntegrateClosedLoop(gyros_rad, accels, mags, accelInertial, magInertial, deltaT);

        // Extract Euler angles from the rotation matrix
        EulerAngles angles = ExtractEulerAngles(R);

        // Convert Euler angles from radians to degrees (inline conversion)
        float yaw_deg = angles.yaw * (180.0f / M_PI);
        float pitch_deg = angles.pitch * (180.0f / M_PI);
        float roll_deg = angles.roll * (180.0f / M_PI);
        // Output Euler angles in degrees
        printf("\rYaw: %5.2f°, Pitch: %5.2f°, Roll: %5.2f°", yaw_deg, pitch_deg, roll_deg);
        fflush(stdout); // Flush the output buffer to ensure it's printed immediately

        HAL_Delay(20); //delay to 50Hz or 20ms
    }
}
#endif