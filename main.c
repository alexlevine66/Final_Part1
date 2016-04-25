// ******************************************************************************************* //
//
// File:         lab3p1.c
// Date:         3-28-2014
// Authors:      Alex Levine
//
// Description: This file takes in an ADC value from the AN0 pin, determined by the position of the
//      potentiometer, and converts it to a digital value. It then takes that file and converts it
//      to a const char* and prints that value to the LCD. It also uses the backwards or forwards
//      function in the pwm.c file to remap the pins to go in one direction or the other. It then
//      sets the OC2RS and OC4RS registers if the forwards function is called, or sets the 
//      OC5RS and OC4RS registers if the backwards function is called. This will allow the wheels to
//      turn in one direction or the other and sets the duty cycle to power the motors.
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

#define LED_3 TRISDbits.TRISD2 // LED3
#define LED3 LATDbits.LATD2

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
    go, left, right, stop, wait, waitf
}stateType;

volatile unsigned int val_1=0;
volatile unsigned int val_2=0;
volatile unsigned int val_3=0;

volatile stateType state = stop;

int main(void)
{   
    SYSTEMConfigPerformance(10000000);
    enableInterrupts();
  
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
    initADC_1();
    initPWM();
    
    writeCMD(CLR);
    
    PINTypeSW = INPUT;
    CNPUDbits.CNPUD6 = 1;
    
    moveCursorLCD(0,2);
    char buf_1[7];
    const char* string_1;
    int IR;
    stateType q = go;
    int flag = 0;
    
    clearLCD();
    
    INPUT1=SEND;
    INPUT3=SEND;
        
    while(1){
        switch(state) {
            case stop:
                accelerator(0,0);
                OC_off();
                if(PORTDbits.RD6==0) // Button released
                    {
                        OC_on();
                        state=wait;
                    }
                break;
            case wait:
                if(PORTDbits.RD6==1) // Button released
                    {
                        state=go;
                    }
                break;
            case waitf:
                if(PORTDbits.RD6==1) // Button released
                    {
                        state=stop;
                    }
                break;
            case go:
                IR = IR_Output();
                if(IR == 2) {
                    accelerator(100,100);
                }
                else if( IR == 1 || IR == 3) {
                    q = left;
                    accelerator(-60,100);
                }
                else if( IR == 4 || IR == 6) {
                    q = right;
                    accelerator(100,-30);
                }
                else if( IR == 7) {
                    accelerator(100,100);
                    if(flag<1) {
                        delayMs(400);
                        while(IR != 2) {
                            accelerator(-100,100);
                            IR = IR_Output();
                        }
                        accelerator(100,100);
                    }
                    flag++;  
                }
               
                else if(IR == 0) {
                    switch(q) {
                        case right:
                            accelerator(100,-30);
                            break;
                        case left:
                            accelerator(-30,100);
                            break;
                        default:
                            q = go;
                            accelerator(50,50);
                            break;
                    }
                }
                if(PORTDbits.RD6==0) // Button released
                    {
                        state=waitf;
                        flag = 0;
                    }
                break;
        }
    }
    return 0;
}


