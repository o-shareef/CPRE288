/**
 * lab6-interrupt_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"  // For scan sensors
#include "uart-interrupt.h"
#include "cyBot_Scan.h"
#include "scan.h"
#include "cyBot_uart.h"
#include "open_interface.h"

// Uncomment or add any include directives that you want to use
// #include "open_interface.h"
 #include "movement.h"
// #include "button.h"

// Your code can use the global variables defined in uart-interrupt.c
// They are declared with the extern qualifier in uart-interrupt.h, which makes the variables visible to this file.

extern volatile char command_byte_s; // byte value for special character used as a command
extern volatile char command_byte_g;
extern volatile int command_flag_s; // flag to tell the main program a special command was received
                                //0 means command not recieved
extern volatile int command_flag_g;


int main(void) {
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	uart_interrupt_init();
	cyBOT_init_Scan(0b0111);
	cyBOT_Scan_t *sensors = malloc(sizeof(float) + sizeof(int));

	// OPTIONAL
	//assign a value to command_byte if you want to know whether that ASCII code is received
	//note that command_byte is global shared variable read by the ISR
	//for example, try using a tab character as a command from PuTTY
	int i = 0;

	while(1)
	{
	    //if we say to go, go
	    if(command_flag_g == 1){
	        command_flag_g = 0;
	        while(command_flag_s == 0){
	            cyBOT_Scan(i, sensors);
	                  i++;

	        }
	        command_flag_s = 0;
	    }

	}

}
