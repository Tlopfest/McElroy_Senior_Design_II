/*
 * FlexCAN.h
 *
 *  Created on: Jan 26, 2022
 *      Author: tlopfest
 */

#ifndef FLEXCAN_H_
#define FLEXCAN_H_
#define NODE_A
void FLEXCAN0_init(void);
void FLEXCAN0_transmit_msg(void);
void FLEXCAN0_receive_msg(void);

#endif /* FLEXCAN_H_ */
