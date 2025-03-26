#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <QEI.h>
#include <pwm.h>
#include <ADC.h>
#include <buttons.h>

static volatile int old_degree = 0;

void QEI_SetColor(int normalized_degree);

int main(void) {
    BOARD_Init();
    QEI_Init();
    PWM_Init();

    while(1) {
        // Get the degree value from the encoder position
        int encoder_position = QEI_GetPosition();
        int encoder_degree = (encoder_position / 4) * 15;
        //printf("Degree: %d\n", encoder_degree);
        //nomralize to 0-360
        int normalized_degree = (encoder_degree % 360 + 360) % 360;
        //printf("normal Degree: %d\n", normalized_degree);

        QEI_SetColor(normalized_degree);
    }
    return 0;
}

//function to set color wheel
void QEI_SetColor(int normalized_degree) {
    if (normalized_degree == 0) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 15) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 25);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 30) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 50);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 45) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 75);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 60) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 75) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 75);
    } else if (normalized_degree == 90) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 50);
    } else if (normalized_degree == 105) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 25);
    } else if (normalized_degree == 120) {
        PWM_SetDutyCycle(PWM_0, 0);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 135) {
        PWM_SetDutyCycle(PWM_0, 25);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 150) {
        PWM_SetDutyCycle(PWM_0, 50);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 165) {
        PWM_SetDutyCycle(PWM_0, 75);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 180) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 100);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 195) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 75);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 210) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 50);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 225) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 25);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 240) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 0);
    } else if (normalized_degree == 255) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 25);
    } else if (normalized_degree == 270) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 50);
    } else if (normalized_degree == 285) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 75);
    } else if (normalized_degree == 300) {
        PWM_SetDutyCycle(PWM_0, 100);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 315) {
        PWM_SetDutyCycle(PWM_0, 75);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 330) {
        PWM_SetDutyCycle(PWM_0, 50);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 100);
    } else if (normalized_degree == 345) {
        PWM_SetDutyCycle(PWM_0, 25);
        PWM_SetDutyCycle(PWM_1, 0);
        PWM_SetDutyCycle(PWM_2, 100);
    }
}
