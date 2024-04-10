/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping_template.h"
#include "cyBot_Scan.h"

// Uncomment or add any include directives that are needed

extern volatile unsigned long START_TIME;
extern volatile unsigned long END_TIME;
extern volatile enum
{
    LOW, HIGH, DONE
} STATE; // State of ping echo pulse

extern volatile bool OVERFLOW;

int main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    ping_init();
    cyBOT_init_Scan(0b0101);
    cyBOT_Scan_t *sensors = malloc(sizeof(float) + sizeof(int));
    unsigned long pastEndTime = END_TIME;
    unsigned long pulseWidth = 0;
    char scanData[40];

    cyBOT_Scan(90, sensors);

    while (1)
    {
        ping_trigger();
        while (END_TIME == pastEndTime)
        {
            //wait for the pulse to come back to sensor
        }
        pulseWidth = START_TIME - END_TIME;
       // sprintf(scanData, "Start: %lu\t     End: %lu\t       PW: %lu\t         Overflow: %s",START_TIME, END_TIME, pulseWidth, OVERFLOW ? "true" : "false");
        float distance = ping_getDistance();
       // sprintf(scanData, "PW: %lu           D: %.3lf", pulseWidth, distance);
        lcd_printf("PW: %lu\n\r Overflow: %s\n\r Distance: %.3lf",pulseWidth, OVERFLOW ? "true" : "false", distance);
        timer_waitMillis(1500);
        //OVERFLOW = false;
    }

}
