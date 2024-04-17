
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
#include "adc.h"

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

    char output[50];

	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	uart_interrupt_init();
	cyBOT_init_Scan(0b0111);
	cyBOT_Scan_t *sensors = malloc(sizeof(float) + sizeof(int));
	oi_t *sensor_data = oi_alloc(); //do this only once at the start of main
	oi_init(sensor_data); // do this only once at the start of main()
	adc_init();
	int sum = 0;
	int value;
	int i = 0;
	int distance = 0;
	uint16_t rawIR = 0;
//	cyBOT_SERVO_cal();
	right_calibration_value = 243250;
	left_calibration_value = 1251250;
	cyBOT_Scan(90, sensors);
	while(i < 15) {
	    rawIR = adc_read();
	    sprintf(output,"RAW IR: %d\t Estimated Distance: %lf\t\n\r", rawIR, calcIRDistance(rawIR));
	    uart_sendStr(output);
	    sprintf(output, "%d    %lf" , rawIR, calcIRDistance(rawIR));
	    lcd_printf(output);
	    timer_waitMillis(100);
	    i++;
	    distance += rawIR;
	}
	    //distance = distance / 15;
	  //sprintf(output,"average = %d\n\n\r", distance);
	 // uart_sendStr(output);


	 oi_free(sensor_data);
	 return 0;


}
