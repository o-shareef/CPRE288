/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping_template.h"
#include "cyBot_Scan.h"
#include "servo.h"

extern unsigned int RIGHT_VAL;
extern unsigned int LEFT_VAL;

// Uncomment or add any include directives that are needed

int main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    //ping_init();
    servo_init();


    RIGHT_VAL = 7500;
    LEFT_VAL = 35000;



    timer_waitMillis(1500);
   servo_move(45);
    timer_waitMillis(1000);
    servo_move(90);
    timer_waitMillis(1000);
    servo_move(135);
    timer_waitMillis(1000);
    servo_move(180);

}
