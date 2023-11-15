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
extern TIM_HandleTypeDef htim3;
extern int sens_rotation;

// I = 1/Sn * (3300/4095 * Vm - V0)
// Sn = 50 mV/A , V0 = 1.65 V , Vm = courant_mes
#define ADC_BUFFER 1
const uint8_t courant[] = "Courant : ";
const uint8_t error_calib[] = "Can't calibrate adc\r\n";
const uint8_t error_start[] = "Can't start adc\r\n";
const uint8_t courant_txt_ma[] = " mA";
const uint8_t vitesse_txt[] = " tour/min";
uint32_t courant_mes;
int32_t courant_mA;
char courant_str[5];
char vitesse_str[5];
uint32_t val_vitesse_t0;
uint32_t val_vitesse_t1;
int32_t val_vitesse;

void Adc_init(void){
	if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED)){
		HAL_UART_Transmit(&huart2, error_calib, 21, HAL_MAX_DELAY);
	}
	if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, &courant_mes, ADC_BUFFER)){
		HAL_UART_Transmit(&huart2, error_start, 17, HAL_MAX_DELAY);
	}
	HAL_TIM_Base_Start(&htim1);
}

void Mes_Courant(void){
	courant_mA = (1000/50)*((courant_mes)*(3300000/4095) - 1650000);
	courant_mA = courant_mA / 1000;
	sprintf(courant_str, "%ld", courant_mA);
	HAL_UART_Transmit(&huart2, courant, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, courant_str, 5, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, courant_txt_ma, 3, HAL_MAX_DELAY);
}

void Adc_2_init(void){
	if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED)){
			HAL_UART_Transmit(&huart2, error_calib, 21, HAL_MAX_DELAY);
		}
		if (HAL_OK != HAL_ADC_Start_DMA(&hadc2, &courant_mes, ADC_BUFFER)){
			HAL_UART_Transmit(&huart2, error_start, 17, HAL_MAX_DELAY);
		}
}

void Mesure_Vitesse(void){
	val_vitesse_t0 = __HAL_TIM_GET_COUNTER(&htim3);
	HAL_Delay(100);
	val_vitesse_t1 = __HAL_TIM_GET_COUNTER(&htim3);
	if (sens_rotation == 1){
		val_vitesse = (val_vitesse_t1 - val_vitesse_t0); // nombre de tick en 0.1 sec
	}else{
		val_vitesse = (val_vitesse_t0 - val_vitesse_t1); // nombre de tick en 0.1 sec
	}
	if (val_vitesse < 0){
		val_vitesse += 65535; // On a dépassé la valeur max du compteur
	}
	// on mesure 102960 tick/s pour alpha = 0.75
	// donc 1 tr/s =~ 4118 tick/s
	val_vitesse = (val_vitesse * 10 * 60) / 4118; // vitesse en tr/min
	sprintf(vitesse_str, "%ld", val_vitesse);
	HAL_UART_Transmit(&huart2, vitesse_str, strlen(vitesse_str), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, vitesse_txt, 9, HAL_MAX_DELAY);
}

