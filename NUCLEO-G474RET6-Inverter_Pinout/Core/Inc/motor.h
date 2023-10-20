/*
 * motor.h
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#include "stm32g4xx_hal.h"
#define MAX_SPEED 999

#define CMD_BUFFER_SIZE 32
#endif /* INC_MOTOR_H_ */

void Start_Motor(char cmd[CMD_BUFFER_SIZE]);
void Stop_Motor(void);
void Change_Speed(char cmd[CMD_BUFFER_SIZE]);

