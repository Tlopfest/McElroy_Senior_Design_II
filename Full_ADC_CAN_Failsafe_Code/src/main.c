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
uint32_t Error_Codes=0;
uint32_t tmp_checker=0;
uint32_t tmp_checker2=0;
uint32_t error_input[100];

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

//double powerOfTen(int num){
//     double rst = 1.0;
//     if(num >= 0){
//         for(int i = 0; i < num ; i++){
//             rst *= 10.0;
//         }
//    }else{
//        for(int i = 0; i < (0 - num ); i++){
//            rst *= 0.1;
//        }
//    }
//
//    return rst;\
//}
//
// double squareRoot(double a)
// {
//     /*
//           find more detail of this method on wiki methods_of_computing_square_roots
//
//           *** Babylonian method cannot get exact zero but approximately value of the square_root
//      */
//      double z = a;
//      double rst = 0.0;
//     int max = 8;     // to define maximum digit
//     int i;
//     double j = 1.0;
//     for(i = max ; i > 0 ; i--){
//         // value must be bigger then 0
//         if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
//         {
//             while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
//             {
//                 j++;
//                 if(j >= 10) break;
//             }
//             j--; //correct the extra value by minus one to j
//             z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z
//             rst += j * powerOfTen(i);     // find sum of a
//             j = 1.0;
//           }
//      }
//
//      for(i = 0 ; i >= 0 - max ; i--){
//          if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
//          {
//              while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
//              {
//                  j++;
//              }
//              j--;
//              z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z
//              rst += j * powerOfTen(i);     // find sum of a
//              j = 1.0;
//           }
//      }
//      // find the number on each digit
//      return rst;
// }
 uint32_t temp_finder(uint32_t voltage){
	//int temperature = slope*voltage+intercept; //Place for formula to find temperature
	//double m = 0.242692;
	//double nx = -13030000;
	//double ox = 76.7463;
	//double b = 31622.8;
	//double v = voltage/1000;
	//double square_root = squareRoot(nx*(v-ox));
	//uint32_t temperature = m*square_root-b;
	double b = 463.87*1000;
	double m = 244.48;
	double v = voltage;
	uint32_t temperature = (m*v-b)/1000;
	return temperature;
}

uint32_t ADC_Reader(uint32_t PORT){
	convertAdcChan(PORT); /* Convert Channel PORT */ //WILL NEED TO SET CORRECT PORT LATER
	while(adc_complete()==0){} /* Wait for conversion complete flag */
	//error_input[counter] = temp_finder(read_adc_chx());
	return temp_finder(read_adc_chx()); //Return voltage input in mv
}

uint32_t averager(uint32_t value1[100]){
	uint32_t value2=0;
	for(int temp=0; temp<100; temp++){
		value2 = value2 + value1[temp];
	}
	return value2/100;
}

uint32_t RTD_Checker(int RTD){
	int PORT=0;
	if(RTD==1)
		PORT=RTD_PORT;
	else if(RTD==2)
		PORT=RTD_PORT2;
	else
		PORT=PTC12;
	for(int counter=0; counter<100; counter++){
		error_input[counter]=ADC_Reader(PORT);
	}
	for(int counter2=0; counter2<100; counter2++){
		if(error_input[counter2] > 700){
			if(RTD==1 && tmp_checker==50)
				Error_Codes=Error_Codes+2;
			else if(RTD==2 && tmp_checker==50)
				Error_Codes = Error_Codes+8;
			else
				tmp_checker++;
		}
		else if(error_input[counter2] < 200){
			if(RTD==1 && tmp_checker2==50)
				Error_Codes=Error_Codes+4;
			else if(RTD==2 && tmp_checker2==50)
				Error_Codes=Error_Codes+16;
			else
				tmp_checker2++;
		}
	}
	for(int counter3=1; counter3<100; counter3++){
		int delta_checker=error_input[counter3-1]-error_input[counter3];
		if((delta_checker > 5 || delta_checker < -5) && RTD==1)
			Error_Codes = Error_Codes+32;
		else if((delta_checker > 5 || delta_checker < -5) && RTD==2)
			Error_Codes = Error_Codes+64;
	}
	return averager(error_input);
}

void CAN_Read(void){
	uint16_t readable=0;
	for(int x=0; x<100000; x++){
		if ((CAN0->IFLAG1 >> 4) & 1)
			readable = 1;
	}
	if (readable==1)
		FLEXCAN0_receive_msg();
	else
		Error_Codes=Error_Codes+1;
}

void Error_Checker(void){
	Error_Codes=0;
	CAN_Read(); //Check CAN readability
	int RTD1=1;
	int RTD2=2;
	uint32_t RTD1_Check=RTD_Checker(0); //Check RTD1 temp value
	//uint32_t RTD2_Check=RTD_Checker(RTD2); //Check RTD2 temp value
	uint32_t RTD2_Check=RTD1_Check; //Temporary for dev board
	if (RTD1_Check > 0xFFFFFFFF || RTD2_Check > 0xFFFFFFFF)
		Error_Codes = Error_Codes+256;
	int delta_checker=RTD1_Check-RTD2_Check;
	if(delta_checker > 5 || delta_checker < -5)
		Error_Codes = Error_Codes+128;
}

void FLEXCAN0_Tester(void){
	FLEXCAN0_transmit_msg_AVG(0xFFFFFFFF+1, -1); //Output should be 00 00 00 00 FF FF FF FF
	FLEXCAN0_transmit_msg_AVG(123, 456); //Output should be 00 00 00 00 7B 00 00 01 C8
}

void averager_Tester(void){
	uint32_t test1[100];
	uint32_t limit = 70000;
	for(int tmp=0;tmp<100;tmp++)
		test1[tmp]=limit;
	uint32_t test2[100];
	for(int tmp=0;tmp<100;tmp++){
		test2[tmp]=tmp;
	}
	uint32_t test_1_avg = averager(test1); //Output should be 70000
	uint32_t test_2_avg = averager(test2); //Output should be 49
}

void Error_Code_Tester(void){
	//uint32_t tester = RTD_Checker(1);
	//uint32_t tester2 = RTD_Checker(2);
	uint32_t tester3 = RTD_Checker(3);
}

int main(void){
	WDOG_disable();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	FLEXCAN0_init();
	PORT_init(); /* Init port clocks and gpio outputs */
	ADC_init();
	FLEXCAN0_Tester();
	averager_Tester();
	Error_Code_Tester();
	Error_Checker();
	for(;;) {
		for(int Error_Counter=0; Error_Counter<10000; Error_Counter++){
			input_1[counter]=ADC_Reader(12);
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
				input_1_avg = averager(input_1);
				input_2_avg = averager(input_2);
				counter=0;
				avg = (input_1_avg+input_2_avg)/2;
				//final_input = temp_finder(avg/1000);
				final_input=avg; //TEMP UNTIL temp_finder IS SETUP
				//convertAdcChan(29); /* Convert chan 29, Vrefsh */
				//while(adc_complete()==0){} /* Wait for conversion complete flag */
				//adcResultInMv_Vrefsh = read_adc_chx(); /* Get channel's conversion results in mv */
				//FLEXCAN0_transmit_msg_test();
				FLEXCAN0_transmit_msg_RTDs(input_1_avg, input_2_avg);
				FLEXCAN0_transmit_msg_AVG(final_input, Error_Codes);
			}
		}
		Error_Checker();
	}
}
