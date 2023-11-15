/*
 * uart.c
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */
#include "shell.h"
#include "main.h"
#include "motor.h"
#include "adc_mes.h"
#include <string.h>

extern UART_HandleTypeDef huart2;

const uint8_t help[] = "Liste des commandes\r\nhelp : donne la liste des commandes\r\npinout : donne la liste des broches connectées\r\nstart : allume le moteur\r\nstop : eteint le moteur\r\n"
		"speed : modifie la vitesse\r\ncourant : affiche le courant en mA\r\nvitesse : affiche la vitesse du moteur";
const uint8_t pinout[] = "PA8 : U\tPB13 : U_barre\r\nPA9 : V\tPB14 : V_barre\r\nPA1 : U_Imes\tPB1 : V_Imes\tBus_Imes : PC2";
const uint8_t powerOn[] = "Powering on the motor";
const uint8_t powerOff[] = "Shutting down the motor";
const uint8_t speedChanged[] = "Changing speed";
const uint8_t cmdNotFound[] = "Command not found";
const uint8_t started[] = "Bienvenue dans ce super shell !";
const uint8_t newLine[] = "\r\n>>>";
int idxCmd = 0;
char cmd[CMD_BUFFER_SIZE];
char buffer_cmd[1];

char* UART_Create_Cmd(void){
	if (buffer_cmd[0] == '\r'){
		if (!(strncmp(cmd,"help",4))){
			HAL_UART_Transmit(&huart2, help, 264, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"pinout",6))){
			HAL_UART_Transmit(&huart2, pinout, 88, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"start",5))){
			Start_Motor(cmd);
			HAL_UART_Transmit(&huart2, powerOn, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"stop",4))){
			Stop_Motor();
			HAL_UART_Transmit(&huart2, powerOff, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"speed",5))){
			HAL_UART_Transmit(&huart2, speedChanged, 14, HAL_MAX_DELAY);
			Change_Speed(cmd);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"courant",7))){
			Mes_Courant();
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"vitesse",7))){
			Mesure_Vitesse();
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

void UART_Echo(void){
	if (buffer_cmd[0] == '\r'){
	  HAL_UART_Transmit(&huart2, newLine, 2, HAL_MAX_DELAY);
	}else{
	  HAL_UART_Transmit(&huart2, buffer_cmd, 1, HAL_MAX_DELAY);
	}
	HAL_UART_Receive_IT(&huart2, buffer_cmd, 1);
}
