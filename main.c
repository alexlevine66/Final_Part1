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
    wait, idle, wait2, forward, wait3, backward, wait4, idle2, 
}stateType;

volatile unsigned int val;

//volatile status state = wait;
//volatile status next;

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
    
    int radix=10;
    writeCMD(CLR);
    
    moveCursorLCD(0,2);
    //testLCD();
    char buf[5];
    const char* string;
    
    
    while(1){
        
        clearLCD();
        if(IFS0bits.AD1IF == 1)
        {
            IFS0bits.AD1IF = 0;
            val=ADC1BUF0;
            //OC2RS=val+500;
            //OC4RS=val+500;
            analog=(3.3*val)/1023;
            sprintf(buf, "%1.1f", analog);
            string=buf;
            printStringLCD(string);
            delayMs(10);
        }
        
        OC4RS=800;
        OC5RS=1000;
        
        
    }
    return 0;
}

//void __ISR(_ADC_VECTOR, IPL7AUTO) _ADCInterrupt(){
  //  IFS0bits.AD1IF = 0;
  //  val = ADC1BUF0;
//}

