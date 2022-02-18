/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h"
#include "clocks_and_modes.h"
#include "ADC.h"
#include "FlexCAN.h"

int REF_VOLT = 5.1;
int led = 0;

#define PTD0 0
#define PTD15 15
#define PTD16 16
#define PTC12 12
#define RTD_PORT 50 //Change this when selecting ports for RTD.
#define RTD_PORT2 51 //Same here

uint32_t input_1[100];
uint32_t input_2[100];
uint32_t input_1_avg = 0;
uint32_t input_2_avg = 0;
uint32_t avg = 0;
uint32_t final_input = 0;
int counter = 0;

void PORT_init (void) {
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[PTD0] = 0x00000100; /* Port D0: MUX = GPIO */
	PORTD->PCR[PTD15] = 0x00000100; /* Port D15: MUX = GPIO */
	PORTD->PCR[PTD16] = 0x00000100; /* Port D16: MUX = GPIO */
	PTD->PDDR |= 1<<PTD0; /* Port D0: Data Direction= output */
	PTD->PDDR |= 1<<PTD15; /* Port D15: Data Direction= output */
	PTD->PDDR |= 1<<PTD16; /* Port D16: Data Direction= output */
	PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
	PORTE->PCR[4] |= PORT_PCR_MUX(5); /* Port E4: MUX = ALT5, CAN0_RX */
	PORTE->PCR[5] |= PORT_PCR_MUX(5); /* Port E5: MUX = ALT5, CAN0_TX */
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

//uint32_t temp_finder(uint32_t voltage){
//	switch (voltage){
//		case (voltage > XXX && voltage < XXX):
//			break;
//		case (voltage > YYY && voltage < YYY):
//			break;
//	}
//}

int main(void){
	WDOG_disable();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	FLEXCAN0_init();
	PORT_init(); /* Init port clocks and gpio outputs */
	ADC_init();
	for(;;) {
		convertAdcChan(12); /* Convert Channel AD12 to pot on EVB */ //WILL NEED TO SET CORRECT PORT LATER
		while(adc_complete()==0){} /* Wait for conversion complete flag */
		input_1[counter] = read_adc_chx();
		if (input_1[counter] > 3750) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD0 | 1<<PTD16; /* turn off blue, green LEDs */
			PTD->PCOR |= 1<<PTD15; /* turn on red LED */
		}
		else if (input_1[counter] > 2500) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD0 | 1<<PTD15; /* turn off blue, red LEDs */
			PTD->PCOR |= 1<<PTD16; /* turn on green LED */
		}
		else if (input_1[counter] >1250) { /* If result > 3.75V */
			PTD->PSOR |= 1<<PTD15 | 1<<PTD16; /* turn off red, green LEDs */
			PTD->PCOR |= 1<<PTD0; /* turn on blue LED */
		}
		else {
			PTD->PSOR |= 1<<PTD0 | 1<< PTD15 | 1<<PTD16; /* Turn off all LEDs */
		}
		//convertAdcChan(RTD_PORT2);
		//while(adc_complete()==0){}
		//input_2 = read_adc_chx();/* Get channel's conversion results in mv */
		input_2[counter]=input_1[counter]; //TEMP FOR POTENTIIOMETER TESTING
		if(counter<100){
			counter++;
		}
		else{
			for(int temp=0; temp<100; temp++){
				input_1_avg = input_1_avg + input_1[temp];
			}
			input_1_avg = input_1_avg/100;
			for(int temp=0; temp<100; temp++){
				input_2_avg = input_2_avg + input_2[temp];
			}
			input_2_avg = input_2_avg/100;
			counter=0;
			avg = (input_1_avg+input_2_avg)/2;
			//final_input = temp_finder(avg/1000);
			final_input=avg; //TEMP UNTIL temp_finder IS SETUP
			//convertAdcChan(29); /* Convert chan 29, Vrefsh */
			//while(adc_complete()==0){} /* Wait for conversion complete flag */
			//adcResultInMv_Vrefsh = read_adc_chx(); /* Get channel's conversion results in mv */
			//FLEXCAN0_transmit_msg_test();
			FLEXCAN0_transmit_msg_RTDs(input_1_avg, input_2_avg);
			FLEXCAN0_transmit_msg_AVG(final_input, 0);
		}
	}
}
