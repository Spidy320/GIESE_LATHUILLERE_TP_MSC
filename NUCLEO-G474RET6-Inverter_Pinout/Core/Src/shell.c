/*
 * uart.c
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */
#include "shell.h"
#include "main.h"
#include "motor.h"
#include <string.h>

const uint8_t help[] = "Liste des commandes\r\nhelp : donne la liste des commandes\r\npinout : donne la liste des broches connectées\r\nstart : allume le moteur\r\nstop : eteint le moteur";
const uint8_t pinout[] = "PA8 : U\r\nPB13 : U_barre\r\nPA9 : V\r\nPB14 : V_barre";
const uint8_t powerOn[] = "Powering on the motor";
const uint8_t powerOff[] = "Shutting down the motor";
const uint8_t speedChanged[] = "Changing speed";
const uint8_t cmdNotFound[] = "Command not found";
const uint8_t started[] = "Bienvenue dans ce super shell !";
const uint8_t newLine[] = "\r\n>>>";
int idxCmd = 0;
char cmd[CMD_BUFFER_SIZE];
char buffer_cmd[1];

char* UART_Create_Cmd(UART_HandleTypeDef huart2,TIM_HandleTypeDef htim1){
	if (buffer_cmd[0] == '\r'){
		if (!(strncmp(cmd,"help",4))){
			HAL_UART_Transmit(&huart2, help, 157, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"pinout",6))){
			HAL_UART_Transmit(&huart2, pinout, 48, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"start",5))){
			Start_Motor(htim1,cmd);
			HAL_UART_Transmit(&huart2, powerOn, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"stop",4))){
			Stop_Motor(htim1);
			HAL_UART_Transmit(&huart2, powerOff, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"speed",5))){
			HAL_UART_Transmit(&huart2, speedChanged, 14, HAL_MAX_DELAY);
			Change_Speed(cmd, huart2, htim1);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, 17, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}
		idxCmd = 0;
	}else{
		cmd[idxCmd] = buffer_cmd[0];
		idxCmd += 1;
		if (idxCmd > CMD_BUFFER_SIZE){
			idxCmd = 0;
		}
	}
	return cmd;
}

void UART_Echo(UART_HandleTypeDef huart2){
	if (buffer_cmd[0] == '\r'){
	  HAL_UART_Transmit(&huart2, newLine, 2, HAL_MAX_DELAY);
	}else{
	  HAL_UART_Transmit(&huart2, buffer_cmd, 1, HAL_MAX_DELAY);
	}
	HAL_UART_Receive_IT(&huart2, buffer_cmd, 1);
}
