#include "PING.h"
#include <stdint.h>
#include "Board.h"
#include "timers.h"
#include "stm32f4xx_hal.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal_tim.h"
#include "sensors.h"

TIM_HandleTypeDef htim3;

volatile int echo_received = 0; // flag to indicate that an echo has been received
volatile uint32_t start_time = 0; // time at which the echo was sent
volatile uint32_t elapsed_time = 0; // time between echo sent and received


char PING_Init(void) {
    // init other libraries
    BOARD_Init();
    TIMER_Init();

    // this block initializes the GPIO output pin (PB8, PWM_5 on shield)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // this block inits the timer generated interrupt
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 83; // divide by 1 prescaler (84-1) = 1 Mhz tick
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 60000; // MUST CHANGE. number of clock cycles between interrupts
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
    HAL_TIM_Base_Start_IT(&htim3); // start interrupt

    // this block inits the external pin-generated interrupt on any change of pin PB5 (ENC_B)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    return SUCCESS;
}
typedef enum { //enum for all possible states
    STATE_TRIGGER_SENSOR,
    STATE_WAIT_FOR_NEXT_CYCLE,
    STATE_WAITING_FOR_TRIGGER,
} State;

volatile State current_state = STATE_TRIGGER_SENSOR;

// external interrupt ISR for pin PB5
void EXTI9_5_IRQHandler(void) {

    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // clear interrupt flag
        // if the pin is low, record the time from last interupt by subtracting the current time from the start time
        //if the pin is high, record the start time
        //pin goes high after trigger is sent, and goes low when echo is received
        if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)) { 
            elapsed_time = TIMERS_GetMicroSeconds() - start_time;
            echo_received = 1;
        } else {
            start_time = TIMERS_GetMicroSeconds();
        }
    }
}




// TIM3 ISR
void TIM3_IRQHandler(void) {
    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag
        switch (current_state) {
            //in this state, the trigger pin is set high, the timer is stopped, the period is set to 10, and the timer is restarted
            case STATE_TRIGGER_SENSOR:
                __HAL_TIM_DISABLE(&htim3); // stop the timer
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  //set the pin high
                htim3.Instance->ARR = 10;  // set new period 
                __HAL_TIM_ENABLE(&htim3); //restart timer
                current_state = STATE_WAITING_FOR_TRIGGER;
                break;
            //in this state, the trigger pin is set low, the timer is stopped, the period is set to 60000, and the timer is restarted
            case STATE_WAITING_FOR_TRIGGER:
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);  //set the pin low
                __HAL_TIM_DISABLE(&htim3);               // stop the timer
                htim3.Instance->ARR = 60000;  // set new period 
                __HAL_TIM_ENABLE(&htim3); //restart timer
                current_state = STATE_WAIT_FOR_NEXT_CYCLE;
                break;
            //if the echo pin recieved a signal then set echo_received to 1, otherwise set elapsed time to 23200 indicating max distance
            case STATE_WAIT_FOR_NEXT_CYCLE:
                if (echo_received) {
                    echo_received = 0;
                    current_state = STATE_TRIGGER_SENSOR;
                }
                else {
                    current_state = STATE_TRIGGER_SENSOR;
                    elapsed_time = 23200; // set to max distance
                }
                break;
        }
    }
}
 #define SCALED_A 18338414 // Scaled value of a
#define SCALED_B  14784775 // Scaled value of b
#define SCALE_FACTOR 1000000 //factor to eliminate floating point

unsigned int PING_GetDistance(void){
    if( elapsed_time > 11765){
        return 400;
    }
    return (elapsed_time * SCALE_FACTOR - SCALED_B) / SCALED_A; //based on the least squares regression
}

unsigned int PING_GetTimeofFlight(void){
    return elapsed_time;
}

