/*
 * ADC.h
 *
 *  Created on: Jan 20, 2022
 *      Author: tlopfest
 */

#ifndef ADC_H_
#define ADC_H_
void ADC1_init(void);
void convertAdcChan1(uint16_t adcChan);
uint8_t adc_complete1(void);
uint32_t read_adc_chx1(void);

#endif /* ADC_H_ */
