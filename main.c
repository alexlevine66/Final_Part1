// ******************************************************************************************* //
//
// File:         lab3p2.c
// Date:         3-28-2014
// Authors:      Alex Levine
//
// Description: This file takes in the ADC value from the AN0 pin and changes it to a const char*
//      which is then able to be printed onto the LCD. Then, it starts in state idle and waits for 
//      the RD6 button to be pushed, once pushed it enters the wait state. The wait state ensures that
//      the button is released and then the state transitions to the forward state. In the forward
//      state, it assigns different values to the OC2RS and OC4RS registers, depending on what the
//      ADC value is, which is obtained from the position of the potentiometer. When the RD6 button
//      is pressed, the state transitions to the wait2 state. There, it waits for the button to be
//      released, and upon release the state goes to the idle2 state which shuts off all OC modules.
//      When the button is pressed again, it transitions to the wait3 state where is ensures that the
//      button is released. Once released, the backwards function of the PWM is called, which remaps the pins,
//      and the state 
//      transitions to the backwards state. In the backward state, it assigns different values to 
//      the OC4RS and OC5RS registers, depending on what the ADC value is, which is obtained from 
//      the position of the potentiometer. When the RD6 button is pressed, it transitions to the 
//      wait4 state, where is checks to ensure that the button is released. Once released, the OC 
//      modules are all shut off and it transitions back to the first idle state, and then repeats.
// ******************************************************************************************* //

#include <xc.h>
#include <sys/attribs.h>
#include "lcd.h"
#include "timer.h"
#include "config.h"
#include "interrupt.h"
#include "switch.h"
#include "keypad.h"
#include "adc.h"
#include "pwm.h"
#include <stdlib.h>


#define CLR 0x01

#define PINTypeIN1 TRISDbits.TRISD10 //J11 Pin 20, RD1
#define PINTypeIN2 TRISDbits.TRISD11 //J11 Pin 14, RC13

#define PINTypeIN3 TRISDbits.TRISD7 // J11 Pin 18, RD3
#define PINTypeIN4 TRISDbits.TRISD8 // J10 Pin 15, RD11

#define PINTypeSW TRISDbits.TRISD6
#define BUTTON PORTDbits.RD6

#define INPUT1 LATDbits.LATD10 // input 1
#define INPUT2 LATDbits.LATD11  // input 2
#define INPUT3 LATDbits.LATD7  // input 3
#define INPUT4 LATDbits.LATD8  // input 4

#define INPUT 1
#define OUTPUT 0

#define SEND 1
#define OFF 0



// ******************************************************************************************* //

typedef enum RS_enum {
    idle, wait, forward, wait2, idle2, wait3, backward, wait4
}stateType;

volatile unsigned int val=0;

volatile stateType state = idle;

int main(void)
{   
    SYSTEMConfigPerformance(10000000);
    enableInterrupts();
    double analog=0;
    int i;
    
    //PINTypeSW=1;
    PINTypeIN1=OUTPUT;
    PINTypeIN2=OUTPUT;
    PINTypeIN3=OUTPUT;
    PINTypeIN4=OUTPUT;
    
    INPUT1=OFF;
    INPUT2=OFF;
    INPUT3=OFF;
    INPUT4=OFF;
 
    initTimer2();
    initLCD();
    initADC();
    initPWM();
    
    writeCMD(CLR);
    
    PINTypeSW = INPUT;
    CNPUDbits.CNPUD6 = 1;
    
    moveCursorLCD(0,2);
    char buf[5];
    char buf2[7];
    const char* string;
    const char* string2;
    
    while(1){
        
        clearLCD();
        
        if(IFS0bits.AD1IF ==1) {
            
            AD1CON2bits.BUFM = 0;
            val = ADC1BUF0;
            
            analog=(3.3*val)/1023;
            
            sprintf(buf, "%1.1f  ", analog);
            string=buf;
            printStringLCD(string);
            
            //sprintf(buf2, "%d", val);
            string2=buf2;
            //printStringLCD(string2);
            delayMs(50);
           
            switch(state) {
                case(idle):
                    OC_off();
                    if(PORTDbits.RD6==0) {
                        changeForwards();
                        state=wait;
                    }
                    break;
                case(wait):
                    if(PORTDbits.RD6==1) {
                        state=forward;
                        INPUT2=OFF;
                        INPUT4=OFF;
                        INPUT1=SEND;
                        INPUT3=SEND;
                    }
                    break;
                case(forward):
                    
                    if(val<512) {
                        OC2RS=1023;
                        OC4RS=val+420;
                    }
                    else {
                        OC2RS=1023-val+420;
                        OC4RS=1023;
                    }
                    
                    if(PORTDbits.RD6==0) {
                        state=wait2;
                    }

                    break;
                case(wait2):
                    if(PORTDbits.RD6==1) {
                        INPUT1=OFF;
                        INPUT3=OFF;
                        INPUT2=OFF;
                        INPUT4=OFF;
                        state=idle2;
                    }
                    break;
                case(idle2):
                    OC_off();
                    if(PORTDbits.RD6==0) {
                        changeBackwards();
                        state=wait3;
                    }
                    break;
                case(wait3):
                    if(PORTDbits.RD6==1) {
                        INPUT1=OFF;
                        INPUT3=OFF;
                        INPUT2=SEND;
                        INPUT4=SEND;
                        state=backward;
                    }
                    break;
                case(backward):
                    if(val<512) {
                        OC4RS=val+420;
                        OC5RS=1023;
                       
                    }
                    else {
                        OC4RS=1023;
                        OC5RS=1023-val+420;
                    }
                    if(PORTDbits.RD6==0) {
                        state=wait4;
                    }
                    break;
                case(wait4):
                    if(PORTDbits.RD6==1) {
                        INPUT1=OFF;
                        INPUT3=OFF;
                        INPUT2=OFF;
                        INPUT4=OFF;
                        state=idle;
                    }
                    break;        
            }
            IFS0bits.AD1IF = 0;
        }
       
    }
    return 0;
}
