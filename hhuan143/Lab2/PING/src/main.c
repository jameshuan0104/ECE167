#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <PING.h>
#include <pwm.h>

float least_squares_regression(float raw_distance, float m, float b) {
    return (raw_distance - b) / m;
}

int main(void) {
    BOARD_Init();
    PING_Init();
    PWM_Init();

    while(1) {
        unsigned int raw_distance = PING_GetDistance();
        HAL_Delay(1000);
        //printf("distance: %d\n",raw_distance);
        float distance = least_squares_regression(raw_distance, 1.017, 2.267);
        //printf("least regression: %f\n", distance);
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetFrequency(distance*5);
    }
    return 0;
}


