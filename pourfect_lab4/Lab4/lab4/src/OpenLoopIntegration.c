#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "MatrixMath.h"
#include "Euler.h"
#include <string.h>
#include "OpenLoopIntegration.h"

#define DEG2RAD(x) ((x) * M_PI / 180.0)  // Convert degrees to radians
#define DT 0.02  // 50Hz update rate (time step)

// Initialize DCM as identity matrix
float R_O[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

// Function to update DCM using forward integration (Euler Method)
void updateDCM(float R[3][3], float p, float q, float r, float dt) {
    // Create skew-symmetric matrix
    float W[3][3] = {
        {  0, -r,  q },
        {  r,  0, -p },
        { -q,  p,  0 }
    };

    // Compute update matrix: W[3x3] * R[3x3]
    float W_R[3][3];
    MatrixMultiply(W, R, W_R);

    // Scale W_R by dt W_R[3x3] *dt
    float dt_W_R[3][3];
    MatrixScalarMultiply(dt, W_R, dt_W_R);

    // Compute new DCM: R_new = R + dt * (W * R)   ad two matrix together 
    float R_new[3][3];
    MatrixAdd(R, dt_W_R, R_new);

    // replace R(K) to R(K+1)
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
             R[i][j] = R_new[i][j];
        }
    }
        
}

void MatrixSubtract(float mat1[3][3], float mat2[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = mat1[i][j] - mat2[i][j];
        }
    }
}

//function to for matrix Exponential
void updateDCM_MatrixExp(float R[3][3], float p, float q, float r, float dt) {
    float I[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };
    float W = sqrt(p*p + q*q + r*r);
    if (W < 1e-6) return;  // Avoid division by zero

    float wx = p / W, wy = q / W, wz = r / W;
    float W_hat[3][3] = {
        {  0,  wz, -wy },
        { -wz,  0,  wx },
        {  wy, -wx,  0 }
    };

    float theta = W * dt;  // ✅ Recommended way
    float sincW, oneMinusCosW;

    if (theta < 0.001f) {  // Small angle approximation
        float theta2 = theta * theta;
        float theta4 = theta2 * theta2;
        sincW = 1.0f - theta2 / 6.0f + theta4 / 120.0f;
        oneMinusCosW = 0.5f - theta2 / 24.0f + theta4 / 720.0f;
    } else {
        sincW = sinf(theta)/theta;
        oneMinusCosW = (1.0f - cosf(theta))/(theta*theta);
    }

    float W_hat2[3][3], sinc_W_hat[3][3], cos_W_hat2[3][3];
    MatrixMultiply(W_hat, W_hat, W_hat2);
    MatrixScalarMultiply(sincW, W_hat, sinc_W_hat);
    MatrixScalarMultiply(oneMinusCosW, W_hat2, cos_W_hat2);

    float temp_W[3][3], exp_W[3][3];
    MatrixSubtract(I, sinc_W_hat, temp_W);
    MatrixAdd(temp_W, cos_W_hat2, exp_W);

    float R_new[3][3];
    MatrixMultiply(exp_W, R, R_new);
    memcpy(R, R_new, sizeof(float)*9);
}


// Function to extract Euler angles from DCM
// void DCMtoEuler(float R[3][3], float *yaw, float *pitch, float *roll) {
//     *yaw = atan2f(R[1][0], R[0][0]) * (180.0 / M_PI);
//     *pitch = asinf(-R[2][0]) * (180.0 / M_PI);
//     *roll = atan2f(R[2][1], R[2][2]) * (180.0 / M_PI);
// }


void OpenLoopIntegrate(float p, float q, float r, float* yaw, float* pitch, float* roll) {
    // Loop to continuously update DCM
        // Update DCM using forward integration
        updateDCM_MatrixExp(R_O, p, q, r, DT);
        // Convert DCM to Euler angles
        DCMtoEuler(R_O, yaw, pitch, roll);
}