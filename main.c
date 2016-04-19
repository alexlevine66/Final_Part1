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
    idle, wait, forward, wait2, idle2, wait3, backward, wait4
}stateType;

volatile unsigned int val_1=0;
volatile unsigned int val_2=0;
volatile unsigned int val_3=0;

volatile stateType state = idle;

int main(void)
{   
    SYSTEMConfigPerformance(10000000);
    enableInterrupts();
    double analog_1=0;
    double analog_2=0;
    double analog_3=0;
    int i=0;
    
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
    //initADC_2();
    //initADC_3();
    initPWM();
    
    writeCMD(CLR);
    
    PINTypeSW = INPUT;
    CNPUDbits.CNPUD6 = 1;
    
    moveCursorLCD(0,2);
    char buf_1[7];
    char buf_2[7];
    char buf_3[7];
    const char* string_1;
    const char* string_2;
    const char* string_3;
    
    while(1){
        
        clearLCD();
        INPUT1=SEND;
        INPUT3=SEND;
        if(IFS0bits.AD1IF ==1) {
            
            
            val_1 = ADC1BUF0;
            val_2 = ADC1BUF2;
            val_3 = ADC1BUF4;

            analog_1=(3.3*val_1)/1023;
            analog_2=(3.3*val_2)/1023;
            analog_3=(3.3*val_3)/1023;
            analog_1=analog_1+0.25;
            
            sprintf(buf_1, "%1.2f  ", analog_1);
            sprintf(buf_2, "%1.2f  ", analog_2);
            sprintf(buf_3, "%1.2f  ", analog_3);
            
            string_1=buf_1;
            string_2=buf_2;
            string_3=buf_3;
            
            printStringLCD(string_1);
            printStringLCD(string_2);
            printStringLCD(string_3);
            
            delayMs(50);
           
            switch(state) {
                case(idle):
                    OC_off();
                    if(PORTDbits.RD6==0) // Button pushed
                    {
                        OC_on();
                        state=wait;
                    }
                    break;
                case(wait):
                    if(PORTDbits.RD6==1) // Button released
                    {
                        state=forward;
                    }
                    break;
                case(forward):
                    
                    if(analog_2 > 2 && analog_1 < 2 && analog_3 < 2) // Going forward, middle sensor on 
                    {
                        OC2RS=750;
                        OC4RS=800;
                    }
                    else if(analog_2 < 2 && analog_1 < 2 && analog_3 < 2) // No sensors activated, move forward
                    {
                        OC2RS=750;
                        OC4RS=800;
                    }
                    else if(analog_2 > 2 && analog_1 > 2 && analog_3 < 2) // Sensor 1 and 2 on, turn left
                    {
                        OC2RS=950;
                        OC4RS=650;
                        delayMs(10);
                    }
                    else if(analog_2 > 2 && analog_1 < 2 && analog_3 > 2) // Sensor 2 and 3 on, turn right
                    {
                        OC2RS=650;
                        OC4RS=950;
                        delayMs(10);
                    }
                    else if(analog_2 > 2 && analog_1 > 2 && analog_3 > 2) // All sensors on, go forward
                    {
                        OC2RS=750;
                        OC4RS=750;  
                    }
                    else if(analog_2 < 2 && analog_1 > 2 && analog_3 < 2) // Only sensor 1 on, turn left hard
                    {
                        OC2RS=950;
                        OC4RS=650;
                        delayMs(25);
                    }
                    else if(analog_2 < 2 && analog_1 < 2 && analog_3 > 2) // Only sensor 3 on, turn hard right
                    {
                        OC2RS=650;
                        OC4RS=950;
                        delayMs(25);
                    }
                    
                    if(PORTDbits.RD6==0) // Button pushed
                    {
                        state=wait2;
                    }

                    break;
                case(wait2):
                    if(PORTDbits.RD6==1) // Button released
                    {
                        state=idle;
                        qw=0;
                    }
                    break;
                
                    
            }
            
          IFS0bits.AD1IF = 0; // Flag down, repeat
          
        }
    }
    return 0;
}


