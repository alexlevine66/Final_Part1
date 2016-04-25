// ******************************************************************************************* //
//
// File:         lab3p1.c
// Date:         3-28-2014
// Authors:      Alex Levine
//
// Description: This initializes the ADC to a 16 bit unsigned integer with auto sampling.
//      It has a prescalar of 4 and scans V ref - and AN0.
// ******************************************************************************************* //

#include <xc.h>
#include "adc.h"

void initADC_1(){
    
    //ANSELBbits.ANSB0 = 0; 
    
    AD1CON1bits.FORM = 0; // 16 unsigned integer
    AD1CON1bits.SSRC = 7; // Auto-convert mode
    AD1CON1bits.ASAM = 1; // Auto-sampling
    AD1CON2bits.VCFG = 0; // Use board refernece voltages
    AD1CON2bits.CSCNA = 1; // Enable scanning
    AD1CON2bits.SMPI = 15; // 1 burrito
    AD1CON2bits.ALTS = 0; // Only Mux A
    AD1CON2bits.BUFM = 0; // 16-bit words
    AD1CON3bits.ADRC = 0; // Use PBCLK
    AD1CON3bits.SAMC = 2; // 2 Tad per sample
    AD1CON3bits.ADCS = 0; // 4 times the PBCLK
    AD1CHSbits.CH0NA = 0; // Use Vref- as negative reference
    
    AD1CSSLbits.CSSL= 0b1111111111111111; // Scan AN0, AN2, and AN4
    
    AD1CON1bits.ADON = 1; // turn on the ADC
}

int IR_Output(void) {

	int val_1;
	int val_2;
	int val_3;
	
	int output = 0;

	if(IFS0bits.AD1IF ==1) {
           
            val_1 = ADC1BUF0 + 50;
            val_2 = ADC1BUF2 - 50;
            val_3 = ADC1BUF4;
			
			if(val_1 > 675) {
				output |= 1;
			}
			if(val_2 > 675) {
				output |= 2;
			}
			if(val_3 > 675) {
				output |= 4;
			}
			
			return output;
			
    }

}