
#ifndef SENSORS_H
#define SENSORS_H

#include "CAPTOUCH.h"
#include "QEI.h"
#include "PING.h"
#include "uart.h"
#include "I2C.h"
#include "BNO055.h"
#include "timers.h"




/**
 * @fn SENSORS_Init
 * @brief Inits I2C, QEI, PING, and UART1 to 9600
 */
void SENSORS_Init(void);

/**
 * @fn SENSORS_cupPresent
 * @brief Returns if cup is present
 * @return 1 if cup is present 0 if no cup present
 */
int SENSORS_cupPresent(void);

/**
 * @fn SENSORS_getWaterHeight
 * @brief Returns height of water based on ping sensor
 * @return Height of the water in mm. 
 */
int SENSORS_getWaterHeight();

/**
 * @fn SENSORS_getPosition
 * @brief Returns current value of the QEI encoder, max 40 and min of 0
 * @return Current value of QEI encoder
 */
int SENSORS_getPosition(void);

/**
 * @fn SENSORS_getCupHeight
 * @brief Returns the cup height based on RX pin reading on UART1. This is info is sent from the esp32.
 * @return Height of the cup, value between 0 - 10
 */
int SENSORS_getCupHeight(void);

/**
 * @fn SENSORS_getWaterLevel
 * @brief Returns the water level as read from water level sensors in water basin
 * @return Water level, can be between 0 - 20
 */
int SENSORS_getWaterLevel(void);

/**
 * @fn SENSORS_checkWaterLevel
 * @brief Effectively a schmitt trigger based on height of water as read by SENSORS_getWaterLevel, if level is above 5 then return 1, if below 2 return 0, if the level
 * is within that range than return the last sent value
 * @return 1 if water level sufficient, 0 otherwise
 */

int SENSORS_checkWaterLevel(void);




#endif

