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
