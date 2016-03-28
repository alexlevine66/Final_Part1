
#include <xc.h>
//#include <proc/p32x470f5121.h>
#include "pwm.h"


void initPWM(){
    RPD1Rbits.RPD1R = 0b1011; // map OC2 to RD1, input 1
    RPD9Rbits.RPD9R = 0;    //input 2
    RPD3Rbits.RPD3R = 0b1011; // map OC4 to RD3, input 3 
    
    RPD11Rbits.RPD11R = 0;   //input 4
    T3CONbits.TCKPS=0;
    PR3=1023;
    TMR3=0;
    T3CONbits.TON=1;
    OC2CONbits.OCTSEL=1;
    OC2CONbits.OCM=6;       // Configure for PWM mode
    OC2RS=0;
    OC2R=0;     // Initialize primary Compare Register
    
    OC4CONbits.OCTSEL=1;
    OC4CONbits.OCM=6;       // Configure for PWM mode
    OC4RS=0;
    OC4R=0;     // Initialize primary Compare Register
    
    OC5CONbits.OCTSEL=1;
    OC5CONbits.OCM=6;       // Configure for PWM mode
    OC5RS=0;
    OC5R=0;     // Initialize primary Compare Register
    
    OC2CONbits.ON=0;
    OC4CONbits.ON=0;
    OC5CONbits.ON=0;

}

void changeForwards() {
    OC2CONbits.ON=0;
    OC4CONbits.ON=0;
    OC5CONbits.ON=0;
    
    RPD1Rbits.RPD1R = 0b1011; // map OC2 to RD1, input 1
    RPD9Rbits.RPD9R = 0;    //input 2
    RPD3Rbits.RPD3R = 0b1011; // map OC4 to RD3, input 3 
    RPD11Rbits.RPD11R = 0;   //input 4
    
    OC2CONbits.ON=1;
    OC4CONbits.ON=1;
    OC5CONbits.ON=0;
}

void changeBackwards() {
    OC2CONbits.ON=0;
    OC4CONbits.ON=0;
    OC5CONbits.ON=0;
    
    RPD1Rbits.RPD1R = 0; // map OC2 to RD1, input 1
    RPD9Rbits.RPD9R = 0b1011;    //input 2
    RPD3Rbits.RPD3R = 0; // map OC4 to RD3, input 3 
    RPD11Rbits.RPD11R = 0b1011;   //input 4
    
    OC2CONbits.ON=0;
    OC4CONbits.ON=1;
    OC5CONbits.ON=1;
    
}

void OC_off() {
    OC2CONbits.ON=0;
    OC4CONbits.ON=0;
    OC5CONbits.ON=0;
}
