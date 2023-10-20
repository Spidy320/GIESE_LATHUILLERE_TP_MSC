/*
 * adc.c
 *
 *  Created on: Oct 20, 2023
 *      Author: colin
 */

#include "main.h"
#include "adc_mes.h"
#include "adc.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart2;

const uint8_t courant[] = "Courant : ";
uint32_t courant_mes;
char courant_str[5];
int val_moy;

void Adc_init(void){
	MX_ADC1_Init();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	for(int i = 0;i < 100; i++){
		val_moy += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	}
	val_moy = val_moy / 100;
}


void Mes_Courant(void){
	courant_mes = HAL_ADC_GetValue(&hadc1) - val_moy;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	sprintf(courant_str, "%ld", courant_mes);
	HAL_UART_Transmit(&huart2, courant, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, courant_str, 5, HAL_MAX_DELAY);
}
