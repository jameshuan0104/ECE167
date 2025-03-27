#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <BNO055.h>
#include "timers.h"
#include "CAPTOUCH.h"
#include "Oled.h"
#include "buttons.h"
#include "QEI.h"
#include "sensors.h"
#include "DFRobot_RGBLCD1602.h"
#include "pwm.h"

typedef enum {
    Level_check,
    Button,
    Pour,
    Done_Pour
}State;

typedef enum {
    Auto_pour,
    Manual_pour
} Pour_mode;

/** startPump()
 *
 * Turns pump on by setting GPIO pin C7 high
 *
 */
void startPump(){
    // Start the pump
    printf("Starting pump...\r\n");
   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);  //set the pin high
}

/** stopPump()
 *
 * Turns pump off by setting GPIO pin C7 low
 *
 */
void stopPump(){
    // Stop the pump
    printf("Stopping pump...\r\n");
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_RESET);  //set the pin high
}


/** checks()
 *
 * Performs checks to ensure cup is present and reserve has enough water
 *
 * @return Returns 0 on failure and 1 on success.
 */
int checks(){
    if(!SENSORS_cupPresent()){
        printf("no cup present\r\n");
        clear();
        printstr("No cup");
        return 0;
    }
    if(!SENSORS_checkWaterLevel()){
        printf("water level is low\r\n");
        clear();
        printstr("Not enough water");
        return 0;
    }
    return 1;
}

//start on level check 
State Current_state =  Level_check;

Pour_mode Pour_type = Auto_pour;



int main(void) {
    BOARD_Init();
    TIMER_Init();
    BUTTONS_Init();
    QEI_Init();
    SENSORS_Init();
    DFRobot_Init();
    PWM_Init();

    //Initialize GPIO pin C7
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_RESET);  
    //Done initializing, make sure pin is low so motor does not start

    uint8_t buttons = 0;
    int pour_amount = 0;
    char buffer[32];  // Buffer to hold the formatted string

    while(1){
        switch(Current_state){ //This is the initial state of our state machine, here a basic check is done to ensure water level is high and a cup is present
            case Level_check:
                HAL_Delay(1000);
                if(!checks()){
                    continue;
                }
                HAL_Delay(100);
                setCursor(0, 0);
                sprintf(buffer, "Water Level: %.0f%%", SENSORS_getWaterLevel() / 2.0 * 10);

                clearPrint(buffer,0);
                printf("moving to button\n");
                Current_state = Button;
            break;

            case Button: //In this state button 4 on the STM is polled, if a call to checks fails we go back to the first state
                pour_amount = SENSORS_getPosition(); //Get the amount to be poured if a manual pour is initiated
                if(!checks()){
                    Current_state = Level_check;
                    continue;
                }
                setCursor(0, 0);
                sprintf(buffer, "Water Level: %.0f%%", SENSORS_getWaterLevel() / 2.0 * 10);
                clearPrint(buffer,0);

                sprintf(buffer, "Amount: %d ", pour_amount);
                setCursor(0, 1);
                clearPrint(buffer,1);
                buttons = buttons_state(); //Get current state of the buttons
                if((buttons & 0x1) == 0){
                    printf("press detected\r\n");
                    for(int i = 0; i <= 100; i++){
                        buttons = buttons_state();
                        if((( buttons & 0x1) == 1) && (i >= 7)){ //If the button press was held for more than ~1 second it is a long press and a manual pour is initiated
                            printf("Long press \r\n");
                            printf("moving to Check state\n");
                            Pour_type = Manual_pour;
                            Current_state = Pour;
                            break; //Go to pour
                        }
                        if(( buttons & 0x1) == 1){ //If the button is held for less than ~1 second initiate a short press
                            printf("short press\r\n");
                            printf("moving to check state\n");
                            Pour_type = Auto_pour;
                            Current_state = Pour;
                            break;  //Go to pour
                        }
                        HAL_Delay(100);
                    }
                }
                HAL_Delay(100);
            break;

            case Pour:  //In this state the machine pours either a manual pour or an auto pour
            printf("in pour state\r\n");
            float last_percent = 0;
            if(Pour_type == Auto_pour){
                printf("pouring automatically\r\n");
                int getCupHeight = SENSORS_getCupHeight(); //Gets height of the cup
                int water_height = 0;
                startPump(); //Starts the pump
                while(water_height < getCupHeight * 3.5){ //Make sure height of water is below height of the cup
                    water_height = SENSORS_getWaterHeight();
                    if(water_height <= 0){ //ensure no negative values
                        water_height = 1;
                    }
                    printf("pouring auto...\r\n");
                    float percent = 100.0 * water_height / (getCupHeight * 3.5); //get percentage of cup filled for printing to screen
                    if (((percent > last_percent) && percent <= 100)  || last_percent == 0){
                    sprintf(buffer, "%.0f%%",  percent);
                    setCursor(0, 1);
                    clearPrint(buffer,1);
                    last_percent = percent;
                    }
                    if(!checks()){ //If a check fails while pouring stop pouring
                        break;
                    }
                    HAL_Delay(200);
                }
                printf("broke out\n");
            } else {
                startPump();
                printf("pouring manually: %d\r\n", pour_amount);
                int count = 0;
                while(count < pour_amount){ //Based on QEI value pour and decrement the count
                    //pour
                    sprintf(buffer, "Amount: %d ", pour_amount - count - 1); //print the count
                    setCursor(0, 1);
                    clearPrint(buffer,1);
                    printf("pouring man...\r\n");
                    count++;
                    if(!checks()){ //If checks fail then stop pouring
                        break;
                    }
                    HAL_Delay(200);
                }
            }
            stopPump(); //Done pouring so stop pump
            Current_state = Done_Pour;
            break;


            case Done_Pour: //Done pouring, moves to check level
            printf("done pouring pour\n moving to check level\n");
            setCursor(0,0);
            clearPrint("Enjoy!",0); //Display finish message
            Current_state = Level_check;
            break;
        }



    }

    return 0;
}