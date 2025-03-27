#include "sensors.h"
#include <stdio.h>
void SENSORS_Init(void) {
    PING_Init();
    QEI_Init();
    I2C_Init();  // Initialize I2C for communication with sensors
    Uart1_Init(9600); //Initialize at 9600 baud rate to communicate with esp32
}




int SENSORS_cupPresent(void) {
    return SENSORS_getCupHeight() ? 1 : 0; //As long as height is not zero there is a cup
}

int SENSORS_getPosition(void) {
    return QEI_GetPosition(); 
}

uint8_t data[10] = {0};  // Buffer for UART data
#define NO_CUP_THRESH 3
int SENSORS_getCupHeight(void){

    int No_cup_count = 0;
    while(No_cup_count < NO_CUP_THRESH){ //keep track of no cup readings, have to have NO_CUP_THRESH readings of no cup in a row to confirm no cup, this is done to limit errors
    if(Uart1_rx(data, 10) == -1){ //read data into buffer
        printf("ERROR\n");
    }  
    int height = 0;
    for(int i = 0; i < 10; i++){
        if(!data[i]){ //A reading of 0 indicates a blocked LED, which indicates an obstruction
            height++;
        } else { //Otherwise a non zero reading indicates an unblocked LED, indicating no obstruction
            No_cup_count++; 
            break;
        }
    }
    if(height){ 
        return height;
    }
}
    //if we get to this point then a 'no cup' reading was read NO_CUP_THRESH times, giving a good indication that there was no cup
    return 0; 
}

#define PING_DEFAULT_DISTANCE 64  // Default distance for water height calculation, this is based on readings from slotted sensor position to bottom of empty cup
int SENSORS_getWaterHeight(void) {
    int distance = PING_GetDistance(); 
    int height = PING_DEFAULT_DISTANCE - distance;  // Example calculation
    HAL_Delay(100);  // Delay for stability
    return height;
}




int water_low = 0;
unsigned char water_level_arr[20] = {0};

//The water level sensors we were using contain 20 pads, the first 8 pads can be read from the register at 0x77, the last 12 from the register at 0x78
int SENSORS_getWaterLevel(void) {
        return 11;
        int waterLevel = 0;
        if(water_low){
            return 0;
        }
        for (int i = 0; i < 20; i++) { //read first 8 bytes
            if(i <= 7){
            water_level_arr[i] = I2C_ReadRegister(0x77, 0x77); 
            }
            else { //read last 12 bytes
                water_level_arr[i] = I2C_ReadRegister(0x77, 0x78);
            }
            waterLevel += water_level_arr[i] != 0;
        }
    return waterLevel;  // Return the calculated height
}

#define WATER_LOW_THRESH 2  //10 percent
#define WATER_HIGH_THRESH 5 //25 percent
int level_low = 1; //0 means water is low, 1 is water high
int SENSORS_checkWaterLevel(void){
    int water_height = SENSORS_getWaterLevel(); //get water level
    //compare water level with our thresholds
    if(water_height < WATER_LOW_THRESH){
        level_low = 0;
    } else if( water_height > WATER_HIGH_THRESH){
        level_low = 1;
    }
    return level_low;
}