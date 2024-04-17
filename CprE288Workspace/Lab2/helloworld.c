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

int main(void)
{

    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
                  // and enables time functions (e.g. timer_waitMillis)

    lcd_init();   // Initialize the LCD screen.  This also clears the screen.

    oi_t *sensor_data = oi_alloc(); //do this only once at the start of main

    oi_init(sensor_data); // do this only once at the start of main()


    move_forward(sensor_data, 2000);
















    /*move_forward(sensor_data, 500); //200 is distance to move in mm
    turn_right(sensor_data, 90);

    move_forward(sensor_data, 500); //200 is distance to move in mm
    turn_right(sensor_data, 90);

    move_forward(sensor_data, 500); //200 is distance to move in mm
    turn_right(sensor_data, 90);

    move_forward(sensor_data, 500); //200 is distance to move in mm
    turn_right(sensor_data, 90);
    */

    oi_free(sensor_data);

    // NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
    // NOTE: For time functions, see Timer.h

    return 0;
}
