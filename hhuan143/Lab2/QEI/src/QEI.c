#include <stdio.h>
#include <stdlib.h>
#include <QEI.h>

static volatile uint8_t last_state;
static volatile signed int encoder_position = 0;

void QEI_Init(void) {
    //Configure GPIO pins : PB4 PB5
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ENC_A | ENC_B;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Enable EXTI interrupts for pins PB4 and PB5
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    // the rest of the function goes here
    //read initial state of A and B, concat to creat 2 bit state
    uint8_t a = HAL_GPIO_ReadPin(GPIOB, ENC_A);
    uint8_t b = HAL_GPIO_ReadPin(GPIOB, ENC_B);
    last_state = (a << 1) | b;
}

void QEI_IRQ(void) {
    // Read the current state of ENC_A and ENC_B
    uint8_t a = HAL_GPIO_ReadPin(GPIOB, ENC_A);
    uint8_t b = HAL_GPIO_ReadPin(GPIOB, ENC_B);
    uint8_t current_state = (a << 1) | b;

    // State machine to determine direction
    switch (last_state) {
        case 0b00:
            if (current_state == 0b01){
                encoder_position--; //ccw
            }else if (current_state == 0b10){
                encoder_position++; //cw
            }
            break;
        case 0b01:
            if (current_state == 0b11){
                encoder_position--;
            }else if (current_state == 0b00){ 
                encoder_position++;
            }
            break;
        case 0b11:
            if (current_state == 0b10){ 
                encoder_position--;
            }else if (current_state == 0b01){
                encoder_position++;
            }
            break;
        case 0b10:
            if (current_state == 0b00){
                encoder_position--;
            }else if (current_state == 0b11){
                encoder_position++;
            }
            break;
    }

    // Update the last state
    last_state = current_state;
}

// External interrupt for pin PB5
void EXTI9_5_IRQHandler(void) {
    // Check and handle interrupt for ENC_B (PB5)
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);  // Clear interrupt flag
        QEI_IRQ();  // Call state machine handler
    }
}

// External interrupt for pin PB4
void EXTI4_IRQHandler(void) {
    // Check and handle interrupt for ENC_A (PB4)
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);  // Clear interrupt flag
        QEI_IRQ();  // Call state machine handler
    }
}


/**
 * @function QEI_GetPosition(void) 
 * @param none
 * @brief This function returns the current position of the Quadrature Encoder in degrees.      
*/
int QEI_GetPosition(void){
    return encoder_position;
}

/**
 * @Function QEI_ResetPosition(void) 
 * @param  none
 * @return none
 * @brief  Resets the encoder such that it starts counting from 0.
*/
void QEI_ResetPosition(){
    encoder_position = 0;
}