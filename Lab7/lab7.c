/*
#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"  // For scan sensors
#include "uart-interrupt.h"
#include "scan.h"
#include "cyBot_uart.h"
#include "open_interface.h"
#include "uart.h"
#include "uart-interrupt.h"
#include "movement.h"

// Your code can use the global variables defined in uart-interrupt.c
// They are declared with the extern qualifier in uart-interrupt.h, which makes the variables visible to this file.

extern volatile char command_byte_s; // byte value for special character used as a command
extern volatile char command_byte_g;
extern volatile int command_flag_s; // flag to tell the main program a special command was received
                                //0 means command not recieved
extern volatile int command_flag_g;


extern float smallestObjectCenterAngle;
extern float smallestObjectDistance;



int main(void) {

    char output[10];
    int sentByte = 0;
    int i = 0;
    bool isS = false;

	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	uart_interrupt_init();
	cyBOT_init_Scan(0b0111);
	cyBOT_Scan_t *sensors = malloc(sizeof(float) + sizeof(int));
	oi_t *sensor_data = oi_alloc(); //do this only once at the start of main
	oi_init(sensor_data); // do this only once at the start of main()


	uart_sendStr("\rREADY!\n\r");
//	cyBOT_SERVO_cal();
	right_calibration_value = 264250;
	left_calibration_value = 1183000;


	sentByte = uart_receive();
	sprintf(output, " got a %c\n\n\r", sentByte);
	    for (i = 0; i < strlen(output); i++) {
	        if (output[7] == 's' || output[7] == 'S') {
	            isS = true;
	            uart_sendStr(output);
	            break;
	        }
	    }



	     //make sure it enters the while loop
	    smallestObjectDistance = 100;

	    while(smallestObjectDistance > 10){
	        cyBOT_proxScan(sensors, sensor_data);

	        sprintf(output, "\n\rAngle of Smallest Object: %.3lf\n\r", smallestObjectCenterAngle);
	                uart_sendStr(output);
	        sprintf(output, "Distance to Smallest Object: %.3lfcm\n\r", smallestObjectDistance);
	        uart_sendStr(output);

	        if(smallestObjectDistance < 10){
	            break;
	        }
	        //changed for demo
	        move_forward(sensor_data, (smallestObjectDistance * 10) - 70);
	    }


	 oi_free(sensor_data);
	 return 0;


}
*/
