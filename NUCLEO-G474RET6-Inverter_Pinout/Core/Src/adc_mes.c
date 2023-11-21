/**
 * \file adc_mes.c
 * \brief mesures du courant et de la vitesse
 * \author Colin L, Lucas G
 * \version 1
 * \date 20 octobre 2023
 */

#include "main.h"
#include "adc_mes.h"
#include "adc.h"
#include <stdio.h>

#define ADC_BUFFER 1

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern int sens_rotation; // savoir dans quel sens tourne le moteur

// Fonction de transfert du courant
// I = 1/Sn * (3300/4095 * Vm - V0)
// Sn = 50 mV/A , V0 = 1.65 V , Vm = courant_mes

const uint8_t error_calib[] = "Can't calibrate adc\r\n"; // messages d'erreur
const uint8_t error_start[] = "Can't start adc\r\n";


uint32_t courant_mes; // endroit ou la valeur du courant est stockée
uint32_t val_vitesse_t0; // permet de mesure la vitesse
uint32_t val_vitesse_t1;
int32_t val_vitesse;

/**
 * \fn void Adc_init(void)
 * \brief Initialisation de l'ADC
 *
 * \param void
 * \return void
 */
void Adc_init(void){ // initialisation de l'ADC et du DMA
	if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED)){
		HAL_UART_Transmit(&huart2, error_calib, 21, HAL_MAX_DELAY);
	}
	if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, &courant_mes, ADC_BUFFER)){
		HAL_UART_Transmit(&huart2, error_start, 17, HAL_MAX_DELAY);
	}
	HAL_TIM_Base_Start(&htim1); // lancement du timer pour la conversion de l'ADC
}

/**
 * \fn int32_t Mes_Courant(void)
 * \brief Mesure le courant et le convertit en mA
 *
 * \param void
 * \return courant_mA la valeur du courant mesuré en mA
 */
int32_t Mes_Courant(void){ // mesure du courant
	int32_t courant_mA = (1000/50)*((courant_mes)*(3300000/4095) - 1650000); // multiplication par 1000 pour eviter les divisions de floats
	courant_mA = courant_mA / 1000; // division par 1000 pour revenir en mA
	return courant_mA;
}

/**
 * \fn int32_t Mesure_Vitesse(void)
 * \brief Mesure la vitesse de rotation du moteur et la convertit en tr/min
 *
 * \param void
 * \return val_vitesse la valeur de la vitesse de rotation mesurée en tr/min
 */
int32_t Mesure_Vitesse(void){ // mesure de la vitesse
	val_vitesse_t0 = __HAL_TIM_GET_COUNTER(&htim3);
	HAL_Delay(100);
	val_vitesse_t1 = __HAL_TIM_GET_COUNTER(&htim3);
	if (sens_rotation == 1){ // permet d'avoir une différence de tick positive (ou on a dépassé le compteur)
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
	return val_vitesse;
}

