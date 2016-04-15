// ******************************************************************************************* //
//
// File:         lab3p2.c
// Date:         3-28-2014
// Authors:      Alex Levine
//
// Description: This initializes the ADC to a 16 bit unsigned integer with auto sampling.
//      It has a prescalar of 4 and scans V ref - and AN0.
// ******************************************************************************************* //
#include <xc.h>
#include "adc.h"

void initADC(){
    
    //ANSELBbits.ANSB0 = 0; 
    AD1CON1bits.FORM = 0; // 16 unsigned integer
    AD1CON1bits.SSRC = 7; // Auto-convert mode
    AD1CON1bits.ASAM = 1; // Auto-sampling
    AD1CON2bits.VCFG = 0; // Use board refernece voltages
    AD1CON2bits.CSCNA = 0; // Disable scanning
    AD1CON2bits.SMPI = 15; // 15 burrito
    AD1CON2bits.ALTS = 0; // Only Mux A
    AD1CON3bits.ADRC = 0; // Use PBCLK
    AD1CON3bits.SAMC = 3; // 2 Tad per sample
    AD1CON3bits.ADCS = 8; // 4 times the PBCLK
    AD1CHSbits.CH0NA = 0; // Use Vref- as negative reference
    AD1CHSbits.CH0SA = 0; // Scan AN0 
    
    AD1CON1bits.ON = 1; // turn on the ADC
}