/**
 * lab4_template.c
 *
 * Template file for CprE 288 lab 4
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 */

#include "button.h"
#include "Timer.h"
#include "lcd.h"
#include "scan.h"
#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1

#define REPLACEME 0



int main(void) {
    char
    p = (char)0b00001000;
    printf("%c",p);
	button_init();
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	cyBot_uart_init();

	char q[1];

	while(1)
	{
	    sprintf(q, "%d", button_getButton());
	    timer_waitMillis(150);
	        if (q[0] == '1') {
	                        timer_waitMillis(100);
	                        cyBot_sendString("\rButton 1 Pressed\r\n");
	                        lcd_printf(q);
	                    }
	        else if (q[0] == '2') {
	                        timer_waitMillis(100);
	                        cyBot_sendString("\rButton 2 Pressed\r\n");
	                        lcd_printf(q);
	                    }
	        else if (q[0] == '3') {
	                        timer_waitMillis(100);
	                        cyBot_sendString("\rButton 3 Pressed\r\n");
	                        lcd_printf(q);
	                    }
	        else if (q[0] == '4') {
	                        timer_waitMillis(100);
	                        cyBot_sendString("\rButton 4 Pressed\r\n");
	                        lcd_printf(q);
	                    }
	    }
	}


