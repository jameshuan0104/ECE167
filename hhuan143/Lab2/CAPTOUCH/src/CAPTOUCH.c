#include <stdio.h>
#include <stdlib.h>
#include <timers.h>
#include <CAPTOUCH.h>

static volatile u_int32_t start_time = 0;
static volatile u_int32_t freq = 0;
static volatile u_int32_t filtered_freq = 0;

#define WINDOW_SIZE 10  // Sample size for moving average filter

int samples[WINDOW_SIZE] = {0};
int sample_index = 0;
int sum = 0;

uint32_t moving_average(uint32_t new_sample) {
    // Subtract the oldest sample from the sum
    sum -= samples[sample_index];
    // Add the new sample
    samples[sample_index] = new_sample;
    sum += new_sample;
    // Update the sample index
    sample_index = (sample_index + 1) % WINDOW_SIZE;
    // Return average
    return sum / WINDOW_SIZE;
}

void CAPTOUCH_Init(void) {
    // Init other libraries
    BOARD_Init();
    TIMER_Init();

    // Configure GPIO pin PB5 
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // EXTI interrupt init
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    // The rest of the function goes here
    start_time = TIMERS_GetMicroSeconds();
}

// External interrupt ISR for rising edge of pin PB5  
void EXTI9_5_IRQHandler(void) {
    // EXTI line interrupt detected 
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        // Clear interrupt flag.
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);

        // Anything that needs to happen on rising edge of PB5 (ENC_B).
        //find the time between two rising edge
        u_int32_t curr_time = TIMERS_GetMicroSeconds();
        freq = curr_time - start_time;
        filtered_freq = moving_average(freq);
        start_time = TIMERS_GetMicroSeconds();        
    }
}

/** CAPTOUCH_IsTouched(void)
 *
 * Returns TRUE if finger is detected. Averaging of previous measurements may
 * occur within this function, however you are NOT allowed to do any I/O inside
 * this function.
 *
 * @return  (char)    [TRUE, FALSE]
 */
char CAPTOUCH_IsTouched(void){
    //sets bounds for when it is touch vs when it it not
    if (filtered_freq < 250){
        return FALSE;
    }else{
        return TRUE;
    }
}



