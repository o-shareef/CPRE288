/*
 * servo.c
 *
 *  Created on: Apr 3, 2024
 *      Author: merwi002
 */
#include "Timer.h"
#include "servo.h"
#define START_VAL 6920
#define END_VAL   35040

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0x2;
     while(!(SYSCTL_RCGCGPIO_R & 0x2));

    /* GPIO_PORTB_AFSEL_R |= 0x8
     GPIO_PORTB_PCTL_R |= 0x70000
     GPIO_PORTB_PCTL_R &= (~0xF000);
     GPIO_PORTB_DEN_R |= 0x8;*/
     GPIO_PORTB_DEN_R |= 0x20;
     GPIO_PORTB_AFSEL_R |= 0x20;
     GPIO_PORTB_PCTL_R &= (~0xF00000);
     GPIO_PORTB_PCTL_R |= 0x700000;
     SYSCTL_RCGCTIMER_R |=0x2;
        while(!(SYSCTL_RCGCTIMER_R & 0x2));


        TIMER1_CTL_R &=(~0x100);
        while(TIMER1_CTL_R & 0x100);

           TIMER1_CFG_R |= 0x4;
           //TBMR periodic timer mode
           TIMER1_TBMR_R &= (~0xFF);
           //TIMER1_TBMR_R |= 0x2;
           //TBCMR edge count mode
           TIMER1_TBMR_R |= 0x1A;
           //TMAMS PWM mode is enabaled
           TIMER1_TBMR_R &= 0x8;
          // TIMER1_TBMR_R &= (~0x10);
           TIMER1_TBMR_R |= 0x7;

           //inverts PWM
           TIMER1_CTL_R |= 0x4000;
           //set to both edges
           TIMER1_CTL_R |= 0xC00;

           //320000

           //enables interupt capture mode
           //TIMER1_TBMR_R |= 0x200;
           //320000
           TIMER1_TBPR_R |= 0x4;

           TIMER1_TBILR_R |= 0xE200;

           TIMER1_TBMATCHR_R = (END_VAL + START_VAL)/2;

           TIMER1_TBMATCHR_R |= 0x000000;

           TIMER1_CTL_R |= 0x100;
           timer_waitMillis(1000);
           TIMER1_CTL_R &=(~0x100);
             // while(!(TIMER1_CTL_R & 0x100));
}

void servo_move(uint16_t degrees){

        TIMER1_CTL_R |= 0x100;
        TIMER1_TBMATCHR_R = (END_VAL - START_VAL)/180 *degrees + START_VAL;
        timer_waitMillis(1000);
        TIMER1_CTL_R &=(~0x100);


}



