// ******************************************************************************************* //
//
// File:         lab1p2.c
// Date:         12-30-2014
// Authors:      Garrett Vanhoy
//
// Description: 
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
            
            sprintf(buf2, "%d", val);
            string2=buf2;
            printStringLCD(string2);
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
