
#include "servo.h"
#include "Timer.h"
#include "lcd.h"
/**
 * main.c
 */
int main(void)
{
    timer_init();
        lcd_init();
        servo_init();

        timer_waitMillis(1000);
        servo_move(30);
        timer_waitMillis(1000);
        servo_move(150);
        timer_waitMillis(1000);
            servo_move(90);


}
