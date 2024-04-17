/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */

#include "Timer.h"
#include "lcd.h"

#include "open_interface.h"
#include "movement.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
#include "scan.h"


int main(void)

{


    char output[10];
    int sentByte = 0;
    int i = 0;
    bool isM = false;
    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
                  // and enables time functions (e.g. timer_waitMillis)

    lcd_init();   // Initialize the LCD screen.  This also clears the screen.

    oi_t *sensor_data = oi_alloc(); //do this only once at the start of main

    oi_init(sensor_data); // do this only once at the start of main()

    cyBot_uart_init();

   // potentialObject *objects = malloc(sizeof(potentialObject));


    //know brogram is running!
    cyBot_sendString("\rThis shit gucci bro\n\r");

    cyBOT_init_Scan(0b0111);
  //  cyBOT_SERVO_cal();  //button 1 and 2 to turn it, button 4 to calibrate, update below values
    right_calibration_value = 285250;
    left_calibration_value = 1272250;

    cyBOT_Scan_t *sensors = malloc(sizeof(float) + sizeof(int));

    sentByte = cyBot_getByte();
    sprintf(output, "got a %c\n\n\r", sentByte);
    for (i = 0; i < strlen(output); i++) {
        if (output[6] == 'm') {
            isM = true;
            cyBot_sendString(output);
            break;
        }
    }

    output[0] = 0;
   if (isM){
       cyBOT_proxScan(sensors);
   }




   //free(objects);


    oi_free(sensor_data);

    // NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
    // NOTE: For time functions, see Timer.h

    return 0;
}
