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

uint32_t input_1 = 0;
uint32_t input_2 = 0;
uint32_t avg = 0;
uint32_t final_input = 0;
char hex[8];

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

//uint32_t temp_finder(uint32_t voltage){
//	switch (voltage){
//		case (voltage > XXX && voltage < XXX):
//			break;
//		case (voltage > YYY && voltage < YYY):
//			break;
//	}
//}

unsigned char int_to_hex(uint32_t value){
	int temp=0;
	while (value > 0){
		switch(value%16){
		case 10:
			hex[temp]='A';
			break;
		case 11:
			hex[temp]='B';
			break;
		case 12:
			hex[temp]='C';
			break;
		case 13:
			hex[temp]='D';
			break;
		case 14:
			hex[temp]='E';
			break;
		case 15:
			hex[temp]='F';
			break;
		default:
			hex[temp]=(value%16)+0x30;
		}
		value=value/16;
		temp++;
		}
	return hex;
}

int main(void){
	WDOG_disable();
	FLEXCAN0_init();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	PORT_init(); /* Init port clocks and gpio outputs */
	ADC_init();
	for(;;) {
		convertAdcChan(RTD_PORT); /* Convert Channel AD12 to pot on EVB */ //WILL NEED TO SET CORRECT PORT LATER
		while(adc_complete()==0){} /* Wait for conversion complete flag */
		input_1 = read_adc_chx();
		convertAdcChan(RTD_PORT2);
		while(adc_complete()==0){}
		input_2 = read_adc_chx();/* Get channel's conversion results in mv */
		avg = (input_1+input_2)/2;
		//final_input = temp_finder(avg/1000);
		final_input=avg/1000; //TEMP UNTIL temp_finder IS SETUP
		unsigned char msg1 = int_to_hex(final_input);
		unsigned char msg2 = msg1;
		//convertAdcChan(29); /* Convert chan 29, Vrefsh */
		//while(adc_complete()==0){} /* Wait for conversion complete flag */
		//adcResultInMv_Vrefsh = read_adc_chx(); /* Get channel's conversion results in mv */
		/*THE FOLLOWING IS PSUDOCODE DUE TO CAN STILL NOT WORKING PROPERLY */
		FLEXCAN0_transmit_msg(msg1, msg2);
		}
	}
