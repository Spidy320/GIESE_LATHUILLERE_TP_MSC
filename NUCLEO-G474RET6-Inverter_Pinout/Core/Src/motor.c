/*
 * motor.c
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */

#include "motor.h"
#include "main.h"
#include "adc_mes.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;

int speed_buffer[4];
const uint8_t maxSpeed[] = "Too fast my friend, too fast";
const uint8_t speedChangedFin[] = "Speed changed";
int sens_rotation = 1;

void Change_Speed(char cmd[CMD_BUFFER_SIZE]){
	/* Debug

	HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, cmd, 10, HAL_MAX_DELAY);
*/
	speed_buffer[0] = cmd[6] - '0';
	speed_buffer[1] = cmd[7] - '0';
	speed_buffer[2] = cmd[8] - '0';
	speed_buffer[3] = cmd[9] - '0';
	int speed = speed_buffer[0]*1000 + speed_buffer[1]*100 + speed_buffer[2]*10 + speed_buffer[3];
	if (speed < 500){
		sens_rotation = 0;
	}else{
		sens_rotation = 1;
	}
	if (speed > MAX_SPEED || speed < 0){
		HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, maxSpeed, 28, HAL_MAX_DELAY);
	}else{
		int speed_actl = __HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1);
		if (speed > speed_actl){
			while (speed_actl < speed){
				speed_actl++;
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,speed_actl);
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,MAX_SPEED + 1 - speed_actl);
				//Mes_Courant();
				//HAL_UART_Transmit(&huart2, "\t", 1, HAL_MAX_DELAY);
				HAL_Delay(25);
			}
		}else{
			while (speed_actl > speed){
				speed_actl--;
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,speed_actl);
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,MAX_SPEED + 1 - speed_actl);
				//Mes_Courant();
				//HAL_UART_Transmit(&huart2, "\t", 1, HAL_MAX_DELAY);
				HAL_Delay(25);
			}
		}
		/*HAL_UART_Transmit(&huart2, speedChangedFin, 13, HAL_MAX_DELAY);*/

	}

}

void Start_Motor(char cmd[CMD_BUFFER_SIZE]){
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,500);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,500);
	cmd[6] = '0'; // Pour éviter speed 6 en première instruction
	cmd[7] = '5'; // Qui n'est pas compris comme 'trop rapide'
	cmd[8] = '0';
	cmd[9] = '0';
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void Stop_Motor(void){
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}
