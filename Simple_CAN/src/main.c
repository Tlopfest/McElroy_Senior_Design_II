#include "S32K144.h"
#include "FlexCAN.h"
#include "clocks_and_modes.h"
void WDOG_disable (void){
	WDOG->CNT=0xD928C520; /* Unlock watchdog */
	WDOG->TOVAL=0x0000FFFF; /* Maximum timeout value */
	WDOG->CS = 0x00002100; /* Disable watchdog */
}
void PORT_init (void) {
	PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
	PORTE->PCR[4] |= PORT_PCR_MUX(5); /* Port E4: MUX = ALT5, CAN0_RX */
	PORTE->PCR[5] |= PORT_PCR_MUX(5); /* Port E5: MUX = ALT5, CAN0_TX */
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[16] = 0x00000100; /* Port D16: MUX = GPIO (to green LED) */
	PTD->PDDR |= 1<<16; /* Port D16: Data direction = output */
}
int main(void) {
	uint32_t rx_msg_count = 0;
	WDOG_disable();
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz(); /* Initialize SPLL to 80 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz SPLL & core, 40 MHz bus, 20 MHz flash */
	FLEXCAN0_init(); /* Init FlexCAN0 */
	PORT_init(); /* Configure ports */
	#ifdef NODE_A /* Node A transmits first; Node B transmits after reception */
	FLEXCAN0_transmit_msg(); /* Transmit initial message from Node A to Node B */
	#endif
	for (;;) { /* Loop: if a msg is received, transmit a msg */
		if ((CAN0->IFLAG1 >> 4) & 1) { /* If CAN 0 MB 4 flag is set (received msg), read MB4 */
			FLEXCAN0_receive_msg (); /* Read message */
			rx_msg_count++; /* Increment receive msg counter */
			if (rx_msg_count == 1000) { /* If 1000 messages have been received, */
				PTD->PTOR |= 1<<16; /* toggle output port D16 (Green LED) */
				rx_msg_count = 0; /* and reset message counter */
			}
			FLEXCAN0_transmit_msg (); /* Transmit message using MB0 */
		}
	}
}

///*
// * main implementation: use this 'C' sample to create your own application
// *
// */
//#include "S32K144.h"
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
