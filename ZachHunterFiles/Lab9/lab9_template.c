/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping.h"

// Uncomment or add any include directives that are needed



int main(void) {
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	ping_init();

	char message[40];
	char widthString[20];

	// YOUR CODE HERE

	while(1)
	{

	  ping_getPulseWidth(widthString, sizeof widthString);
	  //timer_waitMillis(50);
	  sprintf(message, "Distance %.2f \n %s",ping_getDistance(),widthString);
      lcd_printf(message);
     // lcd_clear();
      timer_waitMillis(200);

	}

}
