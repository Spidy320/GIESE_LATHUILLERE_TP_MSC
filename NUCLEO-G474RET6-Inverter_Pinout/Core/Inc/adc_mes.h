/*
 * adc_mes.h
 *
 *  Created on: Oct 20, 2023
 *      Author: colin
 */

#ifndef INC_ADC_MES_H_
#define INC_ADC_MES_H_

#include "stm32g4xx_hal.h"

#endif /* INC_ADC_MES_H_ */

int32_t Mes_Courant(void);
void Adc_init(void);
void DMA_ADC_PrintValue(void);
int32_t Mesure_Vitesse(void);
