#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <CAPTOUCH.h>
#include <pwm.h>



int main(void) {
    BOARD_Init();
    CAPTOUCH_Init();
    PWM_Init();
    while(1) {
        char output = CAPTOUCH_IsTouched();
        //printf("%d\n", output);
    }
    return 0;
}


