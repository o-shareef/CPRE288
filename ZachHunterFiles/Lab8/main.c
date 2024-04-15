#include "adc.h"
#include "Timer.h"
#include "lcd.h"

#include <math.h>

int main()
{
    timer_init();
    lcd_init();
    adc_init();
    int i =0;
    uint16_t temp;
    double dist = 0;

    char message[90];

    while(1)
    {
        for (i =0; i < 5; i++) {
            temp += adc_read();
        }
        temp= temp/6.0;
        dist = 8057.6 * pow(temp,-0.937);
        dist =dist*2.54;
        sprintf(message,"IR: %d, Dis: %lf",temp,dist);
        lcd_printf(message);
        timer_waitMillis(50);
    }
}
