/*
 * FlexCAN.h
 *
 *  Created on: Jan 26, 2022
 *      Author: tlopfest
 */

#ifndef FLEXCAN_H_
#define FLEXCAN_H_
void FLEXCAN0_init(void);
void FLEXCAN0_transmit_msg_test(void);
void FLEXCAN0_receive_msg_test(void);
void FLEXCAN0_transmit_msg_RTDs(int msg1, int msg2);
void FLEXCAN0_transmit_msg_AVG(int msg1, int msg2);

#endif /* FLEXCAN_H_ */
