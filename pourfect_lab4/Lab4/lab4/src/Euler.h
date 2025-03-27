#ifndef EULER_H
#define EULER_H

#include <math.h>

void DCMtoEuler(float dcm[3][3], float *yaw, float *pitch, float *roll);
#endif // EULER_H