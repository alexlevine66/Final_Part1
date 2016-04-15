// ******************************************************************************************* //
//
// File:         lab3p2.c
// Date:         3-28-2014
// Authors:      Alex Levine
//
// Description: This file initializes the PWM, and remaps pins to allow the robot to go
//      forwards and backwards. It also shuts off the output compare modules 2, 4, and 5 to
//      go into the idle state. The PWM uses timer 3 with a prescalar of 1 and a PR value of 1023.
// ******************************************************************************************* //

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif

void initPWM();
void changeForward();
void changeBackward();
void OC_off();

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */
