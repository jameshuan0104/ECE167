#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <pwm.h>
#include <ADC.h>
#include <buttons.h>

#define WINDOW_SIZE 10  // Sample size for moving average filter

int adc_samples[WINDOW_SIZE] = {0};
int sample_index = 0;
int sum = 0;

int moving_average(int new_sample) {
    // Subtract the oldest sample from the sum
    sum -= adc_samples[sample_index];
    // Add the new sample
    adc_samples[sample_index] = new_sample;
    sum += new_sample;
    // Update the sample index
    sample_index = (sample_index + 1) % WINDOW_SIZE;
    // Return average
    return sum / WINDOW_SIZE;
}



int main(void) {
    // Initialize all hardware
    BOARD_Init();
    ADC_Init();
    PWM_Init();

    int time = 80000;
    while (1) {
        // Read raw ADC values
        int raw_flex_val = ADC_Read(ADC_0);
        int filter_flex_val = moving_average(raw_flex_val);
        // printf("%d\n", filter_flex_val);
        double volt_flex_val = ((double)filter_flex_val * 3.3) / (double)4096;
        //printf("Voltage: %f V\n", volt_flex_val); // Corrected format specifier for float

        int raw_piezo_val = ADC_Read(ADC_1);
        //printf("Frequency: %d\n", filter_flex_val);

        double flex_resistance = 47000*((3.3-volt_flex_val) / volt_flex_val);
        //printf("resistance: %f\n",flex_resistance);

        double flex_degree = 3.81*flex_resistance+20700;
        //printf("degree: %f\n",flex_degree);
        if (raw_piezo_val > 55) {
            // Start delay
            PWM_SetDutyCycle(PWM_0, 50);
            PWM_SetFrequency(filter_flex_val);
            //PWM_SetFrequency(flex_degree);
            
            while(time != 0){
                time--;
                raw_piezo_val = ADC_Read(ADC_1);
                if (raw_piezo_val > 55){
                    time = 80000;
                }
            }
        }
        else{
            // Reset the PWM when piezo value is below the threshold
            PWM_SetDutyCycle(PWM_0, 0);
            PWM_SetFrequency(0);
            time = 80000;
        }
    }
}
