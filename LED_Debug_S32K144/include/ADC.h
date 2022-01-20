/*
 * ADC.h
 *
 *  Created on: Jan 20, 2022
 *      Author: tlopfest
 */

#ifndef ADC_H_
#define ADC_H_
void ADC_init(void);
void convertAdcChan(uint16_t adcChan);
uint8_t adc_complete(void);
uint32_t read_adc_chx(void);

#endif /* ADC_H_ */
