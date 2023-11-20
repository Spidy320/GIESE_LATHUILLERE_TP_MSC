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
const uint8_t maxSpeed[] = "Too fast ! 'speed xxxx'";
const uint8_t speedChangedFin[] = "Speed changed";
int sens_rotation = 1;

void Change_Speed(char cmd[CMD_BUFFER_SIZE]){
	/* Debug

	HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, cmd, 10, HAL_MAX_DELAY);
	*/

	// La vitesse est entrée dans le shell avec 4 caractères de 0000 à 1000 pour piloter le
	// rapport cyclique. Chaque caractère est converti en entier avec -'0' et il faut faire
	// attention à rentrer uniquement des chiffres.
	speed_buffer[0] = cmd[6] - '0';
	speed_buffer[1] = cmd[7] - '0';
	speed_buffer[2] = cmd[8] - '0';
	speed_buffer[3] = cmd[9] - '0';

	// Les éléments du buffer sont concaténés pour donner la commande du rapport cyclique.
	int speed = speed_buffer[0]*1000 + speed_buffer[1]*100 + speed_buffer[2]*10 + speed_buffer[3];

	// Pilotage du sens de rotation : rapport cyclique > 500 -> sens de rotation positif
	//												   < 500 -> sens de rotation négatif
	if (speed < 500){
		sens_rotation = 0;
	}else{
		sens_rotation = 1;
	}

	// Si la commande du rapport cyclique n'est pas correcte, renvoie un message d'erreur
	if (speed > MAX_SPEED || speed < 0){
		HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, maxSpeed, 23, HAL_MAX_DELAY);
	}

	// Modification progressive de la vitesse du moteur
	else{
		int speed_actl = __HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1); // rapport cyclique actuel
		if (speed > speed_actl){ // augmente le rapport cyclique envoyé aux PWM
			while (speed_actl < speed){
				speed_actl++;
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,speed_actl); // change les PWM du channel 1
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,MAX_SPEED + 1 - speed_actl); // change les PWM du channel 2
				HAL_Delay(25);
			}
		}else{ // diminue le rapport cyclique envoyé aux PWM
			while (speed_actl > speed){
				speed_actl--;
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,speed_actl); // change les PWM du channel 1
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,MAX_SPEED + 1 - speed_actl); // change les PWM du channel 2
				HAL_Delay(25);
			}
		}
	}
}


void Start_Motor(char cmd[CMD_BUFFER_SIZE]){
	// Initialisation du rapport cyclique du moteur à 500 pour avoir une vitesse nulle au démarrage
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,500);
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,500);

	// On initialise le buffer à 0500 pour avoir une valeur de base et éviter les erreurs
	cmd[6] = '0';
	cmd[7] = '5';
	cmd[8] = '0';
	cmd[9] = '0';

	// Démarage des PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void Stop_Motor(void){
	// Arrêt des PWM
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}
