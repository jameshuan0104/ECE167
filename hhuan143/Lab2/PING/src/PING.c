#include <PING.h>
#include <pwm.h>
#include <stdio.h>
#include <stdlib.h>


static volatile unsigned int echo_start = 0;
static volatile unsigned int echo_end = 0;
static volatile unsigned int echo_time = 0;
static volatile unsigned int distance_mm = 0;
static volatile int current_state = 0;

char PING_Init(void) {
    // Init other libraries
    BOARD_Init();
    TIMER_Init();

    // Initialize GPIO output pin (PB8, PWM_5 on shield)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Initialize timer-generated interrupt
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 83; // Divide by 1 prescaler (84-1) = 1 MHz tick
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF; // MUST CHANGE. Number of clock cycles between interrupts
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        return ERROR;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
        return ERROR;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
        return ERROR;
    }
    HAL_TIM_Base_Start_IT(&htim3); // Start interrupt

    // Initialize external pin-generated interrupt on any change of pin PB5 (ENC_B)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    return SUCCESS;
}


// external interrupt ISR for pin PB5 
void EXTI9_5_IRQHandler(void) {
    // EXTI line interrupt detected
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // clear interrupt flag 
        // anything that needs to happen when PB5 (ENC_B) changes state
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1){
            //rising edge of echo
            echo_start = TIMERS_GetMicroSeconds();
        }else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0){
            //falling edge
            echo_end = TIMERS_GetMicroSeconds();
        }
    }
}

// TIM3 ISR
void TIM3_IRQHandler(void) {
    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag
        switch(current_state) {
            case 0:
                //start pulse wave
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
                __HAL_TIM_SET_AUTORELOAD(&htim3, 10); // 10μs pulse
                current_state = 1;
                break;

            case 1:
                //echo state
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
                __HAL_TIM_SET_AUTORELOAD(&htim3, 60000); // Reset to 60ms
                current_state = 0;
                break;
        }
    }
}

/**
 * @function    PING_GetDistance(void)
 * @brief       Returns the calculated distance in mm using the sensor model determined
 *              experimentally. 
 *              No I/O should be done in this function
 * @return      distance in mm
 */
unsigned int PING_GetDistance(void){
    distance_mm = (echo_time * 340) / 2000; // conversion to mm
    return distance_mm;
}

/**
 * @function    PING_GetTimeofFlight(void)
 * @brief       Returns the raw microsecond duration of the echo from the sensor.
 *              NO I/O should be done in this function.
 * @return      time of flight in uSec
 */
unsigned int PING_GetTimeofFlight(void){
    echo_time = echo_end - echo_start; //raw microsecond
    return echo_time;
}
