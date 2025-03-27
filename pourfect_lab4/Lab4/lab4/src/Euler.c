#include "Euler.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



void DCMtoEuler(float dcm[3][3], float *yaw, float *pitch, float *roll) {
    // Calculate pitch

    //make sure asin argument is between -1 and 1
    if(dcm[2][0] > 1) {
        dcm[2][0] = 1;
    } else if(dcm[2][0] < -1) {
        dcm[2][0] = -1;
    }
    
    *pitch = asin(-dcm[2][0]);
    *pitch = *pitch * (180.0 / M_PI);
    // Check for Gimbal Lock (pitch is near ±90 degrees)
    if (*pitch < 89.5 && *pitch > -89.5){ 
        // Normal case
        *roll = atan2(dcm[2][1], dcm[2][2]);  
        *yaw  = atan2(dcm[1][0], dcm[0][0]);  
        *yaw   = *yaw * (180.0 / M_PI); 
    } else {
        // If gimbal lock, set yaw to 0 and calculate roll
        *roll = atan2(-dcm[1][2], dcm[1][1]);
        //yaw stays unchanged
    }

    *roll  = *roll * (180.0 / M_PI); //convert roll
}