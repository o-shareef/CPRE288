/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping_template.h"
#include "Timer.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse
volatile bool OVERFLOW = false;

void ping_init (void){

    SYSCTL_RCGCTIMER_R |= 0b001000;//send clock to timer
    SYSCTL_RCGCGPIO_R |= 0b000010; //send clock to gpio B
    while((SYSCTL_PRGPIO_R & 0b10) != 0b10) {}; //wait for stabilization


    GPIO_PORTB_DEN_R |= 0b00001000;//do stuff with pb3
    GPIO_PORTB_DIR_R |= 0b00001000;
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    GPIO_PORTB_PCTL_R |= 0x00007000;
    // Configure and enable the timer

    TIMER3_CTL_R &= ~0b100000000; //clear bit 8 to disable timer

    // clear previous cfg
    TIMER3_CFG_R |= 0x4; //SET TO 16 BIT TIMER :D

    TIMER3_TBMR_R &= ~0b11000; //make sure we're counting down
    TIMER3_TBMR_R |= 0x07; //CONFIGURE TO EDGE-TIME MODE AND CAPTURE MODE
    TIMER3_CTL_R |= 0b110000000000; //set timer b to be in both edge event mod3
    TIMER3_TBILR_R = 0xFFFF; //max value of timer thingy
    TIMER3_TBPR_R = 0xFF; //another prescaler thing (IF CODE DON'T WORK MAY BE CULPRIT)

    TIMER3_ICR_R = 0b10000000000; //CLEAR TIMER 3B CAPTURE MODE EVENT INTERRUPT
    TIMER3_IMR_R |= 0b10000000000; //enable capture mode event interrupt

//    NVIC_PRI9_R &= ~0b1010000;//SET PRIORITY OF INTERRUPT 36
//    NVIC_PRI9_R |= 0b0100000;


    //ENABLE TIMER 3 AGAIN
    TIMER3_CTL_R |= 0b100000000; //enable the timer again

    NVIC_EN1_R |= 0x00000010; //ENABLE INTERRUPT 36
    IntRegister(INT_TIMER3B, TIMER3B_Handler);
    IntMasterEnable();
}

void ping_trigger(void){
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0b100000000; //clear bit 8 to disable timer
    TIMER3_IMR_R &= ~0b10000000000;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0b00001000;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    //start pulse sent by mc as digital output on pb3.

    //make the pin low high low, (write 0 - 1 - 0 to the gpiodata register bit. Refer to ping datasheet for the dururation of how long
    //it should be high
    GPIO_PORTB_DATA_R = 0b0000;
    GPIO_PORTB_DATA_R = 0b1000;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R = 0b0000;
    //CHANGE STATE ENUM and clear overflow trigger bool too
    STATE = LOW;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0b10000000000;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    TIMER3_IMR_R |= 0b10000000000;
    TIMER3_CTL_R |= 0b100000000;
}

void TIMER3B_Handler(void){

    // YOUR CODE HERE
    // As needed, go back to review your interrupt handler code for the UART lab.
    // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
    // includes checking the source of the interrupt and clearing the interrupt status bit.
    // Checking the source: test the MIS bit in the MIS register (is the ISR executing
    // because the input capture event happened and interrupts were enabled for that event?
    // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
    // The rest of the code in the ISR depends on actions needed when the event happens.

    /*The ISR should read the timer register and complete its
     processing fast enough to avoid loss of events.
     17*/

    //check if handler called due to an edge capture event
    if (TIMER3_MIS_R & 0b10000000000)
    {
        //byte was received in the UART data register
        //clear the RX trigger flag (clear by writing 1 to ICR)
        TIMER3_ICR_R |= 0b10000000000;

        //if start time is zero, we know it hasn't seen the rising edge of the echo pulse yet
        if(STATE == LOW)
        {
            //set the starting value
            START_TIME = TIMER3_TBR_R;
            STATE = HIGH;
        }
        //if the start time is greater than the current time in the register, we know it's the falling edge so we categorize it accordingly
        else if (STATE == HIGH)
        {
            END_TIME = TIMER3_TBR_R;
            STATE = DONE;

            //check for overflow
            if(TIMER3_TBR_R > START_TIME){
                //add val to start time to account for overflow
                START_TIME += 0xFFFFFF;
                OVERFLOW = true;
            }
        }
    }


}

float ping_getDistance (){
    //16 MHZ = 16000000 cycles / sec
    //34,300 cm / sec = speed of sound
    float distance = ((((START_TIME - END_TIME) * 34300.0) / 16000000.0) / 2.0);

    return distance;

}
