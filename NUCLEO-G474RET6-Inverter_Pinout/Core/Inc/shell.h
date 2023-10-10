/*
 * uart.h
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32g4xx_hal.h"
#define CMD_BUFFER_SIZE 32
#define UART_RX_BUFFER_SIZE 32
#define UART_TX_BUFFER_SIZE 32

#endif /* INC_UART_H_ */


char* UART_Create_Cmd(UART_HandleTypeDef huart2,TIM_HandleTypeDef htim1);
void UART_Echo(UART_HandleTypeDef huart2);
