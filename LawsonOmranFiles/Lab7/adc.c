/*
 * adc.c
 *
 *  Created on: Mar 20, 2024
 *      Author: shareef
 */


#include "adc.h"
#include "uart.h"
#include "uart-interrupt.h"
#include <stdint.h>
#include <math.h>


//USING PB4 SS3

void adc_init() {
    SYSCTL_RCGCADC_R |= 0x0001; //activate ADC0
    SYSCTL_RCGCGPIO_R |= 0b10;  //activate clock for port B
    while((SYSCTL_PRGPIO_R & 0b10) != 0b10) {}; //for stabilization
    GPIO_PORTB_DIR_R &= ~0x10; //make PB4 input
    GPIO_PORTB_AFSEL_R |= 0x10; //enable alternate function on PB4
    GPIO_PORTB_DEN_R &= ~0x10; //disable digital i/o in OB4
    GPIO_PORTB_AMSEL_R |= 0x10; //enable analog on PB4
    while((SYSCTL_PRADC_R & 0x0001) != 0x0001) {}; //wait for ADC to be ready
    GPIO_PORTB_ADCCTL_R &= ~0xFF; //ADC control clear for trigger


    ADC0_ACTSS_R &= ~0x0008; //disable SS3
    ADC0_EMUX_R &= ~0xFFFF; //SS3 is software trigger
    ADC0_SSMUX3_R &= ~0x000F; //set channel
    ADC0_SSMUX3_R += 10;
    ADC0_SSCTL3_R = 0x0006; //no TS0, no D0, yes IE1, yes END0
    ADC0_ACTSS_R |= 0x0008; //enable SS3
}


uint16_t adc_read() {
    uint16_t result;
    ADC0_PSSI_R = 0x0008; //initiate SS3
    while((ADC0_RIS_R & 0x08) == 0) {}; //wait for conversion

    result = ADC0_SSFIFO3_R & 0xFFF; //read result
    ADC0_ISC_R = 0x0008; //interrupt status and clear, acknowledge completion


    return result;
}


float calcIRDistance(uint16_t rawIR){
    //equation we found :D
    return (3655197.20350738)/(pow(rawIR, 1.62866449));
}
