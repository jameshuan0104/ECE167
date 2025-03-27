#include "QEI.h"
#include "timers.h"
#include <stdio.h>
volatile int count = 0;
volatile int count_out = 0;

void QEI_Init(void) {
      //Configure GPIO pins : PB4 PA15
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = GPIO_PIN_4;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      GPIO_InitStruct.Pin = GPIO_PIN_15; //Had to change this to be pin A15 for this project
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      // EXTI interrupt init
      HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(EXTI4_IRQn);
      HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  

      // the rest of the function goes here

      TIMER_Init();
 }

typedef enum { //enum for all possible states
    STATE_00, //Both A and B low
    STATE_10, //A high B low
    STATE_01, //A low B high
    STATE_11 //Both A and B high
} State;

volatile State current_state_QEI = STATE_00;  //keeps track of current states


typedef enum { //enum for channel values of A and B
    EVENT_B_HIGH,
    EVENT_A_HIGH,
    EVENT_B_LOW,
    EVENT_A_LOW,
} Event;


  void QEI_IRQ() {
     /*
     In order to reduce error in the pins bouncing, an internal count is kept which is incremented or decremented on every valid state change.
     When this count gets to 4 the external count is incremented by 1, if it gets to -4 the external count is decremented by 1.
     */
        Event a_state;
        Event b_state;
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)) { //check if b pin is high or low
            b_state = EVENT_B_HIGH;
        } else {
            b_state = EVENT_B_LOW;
        }

        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)) { //check if a pin is high or low
            a_state = EVENT_A_HIGH;
        } else {
            a_state = EVENT_A_LOW;
        }

    switch (current_state_QEI) {
        case STATE_00:
            if (a_state == EVENT_A_HIGH && b_state == EVENT_B_LOW) {  //clockwise
            count++; 
            current_state_QEI = STATE_10;
            }
            else if (b_state == EVENT_B_HIGH && a_state == EVENT_A_LOW) { //counter clockwise
                current_state_QEI = STATE_01;
                count--;
            }
            break;
        case STATE_01:
            if (a_state == EVENT_A_HIGH && b_state == EVENT_B_HIGH) { //counter clockwise
                current_state_QEI = STATE_11;
                count--;
            }
            else if (a_state == EVENT_A_LOW && b_state == EVENT_B_LOW) { //clockwise
                current_state_QEI = STATE_00;
                count++;
            }
            break;
        case STATE_10:
            if (a_state == EVENT_A_LOW && b_state == EVENT_B_LOW) { //counter clockwise
                current_state_QEI = STATE_00;
                count--;
            }
            else if (b_state == EVENT_B_HIGH && a_state == EVENT_A_HIGH) { //clockwise
            current_state_QEI = STATE_11;
            count ++;
            }
            break;
        case STATE_11:
            if (a_state == EVENT_A_LOW && b_state == EVENT_B_HIGH) { //clockwise
                current_state_QEI = STATE_01; 
                count ++;
            }
            else if (b_state == EVENT_B_LOW && a_state == EVENT_A_HIGH) { //counter clockwise
            current_state_QEI = STATE_10;
            count --;
            }
            break;
        default:
           break;
    }
    //check if count is 4 or -4 and update count_out
    if(count == 4){
        count = 0;
        count_out++;
        if(count_out > 40){
            count_out = 40; //prevent count_out from going over 40
        }
    } else if( count == -4){
        count = 0;
        count_out--;
        if(count_out < 0){ //prevent count_out from going negative
            count_out = 0;
        }
    }
 }


// SYSCFG external interrupt ISR for pin PA15.
void EXTI15_10_IRQHandler(void) {
    // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15); // Clear interrupt flag.
        QEI_IRQ();
    } else 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) == RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15); // Clear interrupt flag.
        QEI_IRQ();
    }
}


 volatile uint32_t last_a_time = 0;


 // external interrupt ISR for pin PB4
void EXTI4_IRQHandler(void) {
     // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4); // clear interrupt flag
            QEI_IRQ();
    }
 }
 

 
int QEI_GetPosition(void){
    return count_out; //return the position of the encoder in increments of 15 degrees
}

//reset the count and count_out
void QEI_ResetPosition(){
    count = 0;
    count_out = 0;
}
