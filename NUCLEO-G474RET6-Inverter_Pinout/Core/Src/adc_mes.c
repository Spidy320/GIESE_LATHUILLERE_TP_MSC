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
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

// I = 1/Sn * (3300/4095 * Vm - V0)
// Sn = 50 mV/A , V0 = 1.65 V , Vm = courant_mes
#define ADC_BUFFER 1
const uint8_t courant[] = "Courant : ";
const uint8_t error_calib[] = "Can't calibrate adc\r\n";
const uint8_t error_start[] = "Can't start adc\r\n";
uint32_t courant_mes;
char courant_str[5];
int print_courant = 0;
int val_moy = 0 ;

void Adc_init(void){
	if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED)){
		HAL_UART_Transmit(&huart2, error_calib, 21, HAL_MAX_DELAY);
	}
	if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, &courant_mes, ADC_BUFFER)){
		HAL_UART_Transmit(&huart2, error_start, 17, HAL_MAX_DELAY);
	}
	HAL_TIM_Base_Start(&htim1);
	/*
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	for(int i = 0;i < 100; i++){
		val_moy += HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	}
	val_moy = val_moy / 100;
	*/
}

void Mes_Courant(void){
	courant_mes = (1/50)*((courant_mes)*(3300/4095) - 1650);
	sprintf(courant_str, "%ld", courant_mes);
	HAL_UART_Transmit(&huart2, courant, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, courant_str, 5, HAL_MAX_DELAY);

	/*
	courant_mes = HAL_ADC_GetValue(&hadc1) - val_moy;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	sprintf(courant_str, "%ld", courant_mes);
	HAL_UART_Transmit(&huart2, courant, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, courant_str, 5, HAL_MAX_DELAY);
	*/
}

void DMA_ADC_PrintValue(void){

}


