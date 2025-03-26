#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <pwm.h>
#include <ADC.h>
#include <buttons.h>

#define WINDOW_SIZE 10  // Number of samples for averaging

int adc_samples[WINDOW_SIZE] = {0};
int sample_index = 0;
int sum = 0;

int moving_average(int new_sample) {
    // Subtract the oldest sample from the sum
    sum -= adc_samples[sample_index];
    // Add the new sample
    adc_samples[sample_index] = new_sample;
    sum += new_sample;
    // Update the sample index (circular buffer)
    sample_index = (sample_index + 1) % WINDOW_SIZE;
    // Return the average
    return sum / WINDOW_SIZE;
}


int main(void) {
    BOARD_Init();
    ADC_Init();
    PWM_Init();
    BUTTONS_Init();
    while(1){
        int raw_pot_val = ADC_Read(ADC_CHANNEL_4);
        int filter_pot_val = moving_average(raw_pot_val);
        printf("raw pot value: %d\n", raw_pot_val);
        printf("filtered pot value: %d\n", filter_pot_val);
        PWM_SetDutyCycle(PWM_0, 50);
        PWM_SetFrequency(3000);
        if (buttons_state() == 7){
            PWM_SetDutyCycle(PWM_0, 1);
            PWM_SetFrequency(filter_pot_val);
        }else if (buttons_state() == 11){
            PWM_SetDutyCycle(PWM_0, 25);
            PWM_SetFrequency(filter_pot_val);
        }else if (buttons_state() == 13){
            PWM_SetDutyCycle(PWM_0, 75);
            PWM_SetFrequency(filter_pot_val);
        }else if (buttons_state() == 14){
            PWM_SetDutyCycle(PWM_0, 100);
            PWM_SetFrequency(filter_pot_val);
        }
    }
}

