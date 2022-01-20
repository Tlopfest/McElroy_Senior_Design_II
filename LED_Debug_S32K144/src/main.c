/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h"
#include "clocks_and_modes.h"
#include "ADC.h"

int REF_VOLT = 5.1;
int led = 0;

#define PTD0 0
#define PTD15 15
#define PTD16 16
#define PTC12 12

uint32_t adcResultInMv_pot = 0;
uint32_t adcResultInMv_Vrefsh = 0;

void PORT_init (void) {
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[PTD0] = 0x00000100; /* Port D0: MUX = GPIO */
	PORTD->PCR[PTD15] = 0x00000100; /* Port D15: MUX = GPIO */
	PORTD->PCR[PTD16] = 0x00000100; /* Port D16: MUX = GPIO */
	PTD->PDDR |= 1<<PTD0; /* Port D0: Data Direction= output */
	PTD->PDDR |= 1<<PTD15; /* Port D15: Data Direction= output */
	PTD->PDDR |= 1<<PTD16; /* Port D16: Data Direction= output */
}

void WDOG_disable(void){
	WDOG -> CNT = 0xD928C520; //unlock watchdog
	WDOG -> TOVAL = 0x0000FFFF; //max timeout
	WDOG -> CS = 0x00002100; //disable watchdog
}

void Power_LED(int test){
	if(test){
		PTD->PCOR |= 1<<PTD0;
	}
	else{
        PTD->PSOR |= 1<<PTD0;
	}
}

//int ADC_Reader(){
//	float input = //ADC;
//	float voltage = (input/2^12)*REF_VOLT;
//	int final_voltage = (int)(voltage+0.5);
//	return final_voltage;
//}

int main(void){
	WDOG_disable();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	PORT_init(); /* Init port clocks and gpio outputs */
	ADC_init();
	//PCC-> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
	//PTD->PDDR |= 1<<PTD0;
	//PORTD->PCR[0] = 0x00000100;
	//for(int input = 0; input < 10000; input++){
	//int input = ADC_Reader();
	//if(input%2==0)
	//	led=1;
	//else
	//	led=0;
	//Power_LED(led);
	//for(int x = 0; x<1000; x++);
	for(;;) {
		convertAdcChan(12); /* Convert Channel AD12 to pot on EVB */
		while(adc_complete()==0){} /* Wait for conversion complete flag */
			adcResultInMv_pot = read_adc_chx(); /* Get channel's conversion results in mv */
		if (adcResultInMv_pot > 3750) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD0 | 1<<PTD16; /* turn off blue, green LEDs */
			PTD->PCOR |= 1<<PTD15; /* turn on red LED */
		}
		else if (adcResultInMv_pot > 2500) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD0 | 1<<PTD15; /* turn off blue, red LEDs */
			PTD->PCOR |= 1<<PTD16; /* turn on green LED */
		}
		else if (adcResultInMv_pot >1250) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD15 | 1<<PTD16; /* turn off red, green LEDs */
			PTD->PCOR |= 1<<PTD0; /* turn on blue LED */
		}
		else {
			PTD->PSOR |= 1<<PTD0 | 1<< PTD15 | 1<<PTD16; /* Turn off all LEDs */
		}
		convertAdcChan(29); /* Convert chan 29, Vrefsh */
		while(adc_complete()==0){} /* Wait for conversion complete flag */
		adcResultInMv_Vrefsh = read_adc_chx(); /* Get channel's conversion results in mv */
		}
	}
//
//#if defined (__ghs__)
//    #define __INTERRUPT_SVC  __interrupt
//    #define __NO_RETURN _Pragma("ghs nowarning 111")
//#elif defined (__ICCARM__)
//    #define __INTERRUPT_SVC  __svc
//    #define __NO_RETURN _Pragma("diag_suppress=Pe111")
//#elif defined (__GNUC__)
//    #define __INTERRUPT_SVC  __attribute__ ((interrupt ("SVC")))
//    #define __NO_RETURN
//#else
//    #define __INTERRUPT_SVC
//    #define __NO_RETURN
//#endif
//
//int counter, accumulator = 0, limit_value = 1000000;
//
//int main(void) {
//    counter = 0;
//
//    for (;;) {
//        counter++;
//
//        if (counter >= limit_value) {
//            __asm volatile ("svc 0");
//            counter = 0;
//        }
//    }
//    /* to avoid the warning message for GHS and IAR: statement is unreachable*/
//    __NO_RETURN
//    return 0;
//}
//
//__INTERRUPT_SVC void SVC_Handler() {
//    accumulator += counter;
//}
