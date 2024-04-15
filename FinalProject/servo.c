/*
 * servo.c
 *
 *  Created on: Apr 3, 2024
 *      Author: shareef & lawson
 */

#include "Timer.h"
#include "servo.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

unsigned int RIGHT_VAL = 0;
unsigned int LEFT_VAL = 0;

void servo_init(void)
{
    SYSCTL_RCGCTIMER_R |= 0b10; //send clock to timer
    SYSCTL_RCGCGPIO_R |= 0b10; //send clock to gpio B
    while ((SYSCTL_PRGPIO_R & 0b10) != 0b10)
    {
    }; //wait for stabilization

    GPIO_PORTB_DEN_R |= 0b100000; //initialize PB5
    GPIO_PORTB_DIR_R |= 0b100000;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CTL_R &= ~0b100000000; //clear bit 8 to disable timer

    TIMER1_CFG_R |= 0x4; //SET TO 16 BIT TIMER :D
    TIMER1_TBMR_R &= ~0b100; //config for PWM
    TIMER1_TBMR_R |= 0b1010; //More pulse width config stuff
    TIMER1_TBPR_R = 0x04; //add upper hex val of 04 to prescale
    TIMER1_TBILR_R = 0xE200; //upper value of e200 which is smushed together with prescaler to make 04e200

    //todo next time - add prescale match register congif for 0 degrees (1 ms at high)
    //figure out normal register match val for 0 degrees
    TIMER1_TBPMR_R = 0x04;
    //match to clock cycle count of 0 degrees
    TIMER1_TBMATCHR_R = 0xC4B4;

    //re-enable timer
    TIMER1_CTL_R |= 0b100000000; //set bit 8 to enable timer

} //for speed do waitmillis(abs(previous - current) * 5

void servo_move(uint16_t degrees)
{
    int clockCyclesFromMax = (((LEFT_VAL - RIGHT_VAL) / 180) * degrees) + RIGHT_VAL;
    int matchValues = 320000 - clockCyclesFromMax;
    //set clock cycles
    TIMER1_TBPMR_R = (matchValues >> 16);
    //match to clock cycle count of 0 degrees
    TIMER1_TBMATCHR_R = (matchValues &= ~0x110000);

}
