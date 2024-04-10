/*
*
*   uart.c
*
*
*
*   @author
*   @date
*/

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "uart.h"
#include "lcd.h"

void uart_init(void){


	//TODO
  //enable clock to GPIO port B
  SYSCTL_RCGCGPIO_R |= 0b000010;

  //enable clock to UART1
  SYSCTL_RCGCUART_R |= 0b00000010;

  //wait for GPIOB and UART1 peripherals to be ready
  while ((SYSCTL_PRGPIO_R & 0b000010) == 0) {};
  while ((SYSCTL_PRUART_R & 0b00000010) == 0) {};

  //enable alternate functions on port B pins
  GPIO_PORTB_AFSEL_R |= 0b00000011;

  //enable digital functionality on port B pins
  GPIO_PORTB_DEN_R |= 0b00000011;

  //enable UART1 Rx and Tx on port B pins
 // GPIO_PORTB_PCTL_R = ???
  GPIO_PORTB_PCTL_R &= 0xFFFFFF00;
  GPIO_PORTB_PCTL_R |= 0x00000011;

  //calculate baud rate
  uint16_t iBRD = 0x8; //use equations
  uint16_t fBRD = 0x2C; //use equations

  //turn off UART1 while setting it up
  UART1_CTL_R &= ~0b1;

  //set baud rate
  //note: to take effect, there must be a write to LCRH after these assignments
  UART1_IBRD_R = iBRD;
  UART1_FBRD_R = fBRD;

  //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
  //note: this write to LCRH must be after the BRD assignments
  UART1_LCRH_R = 0b01100000;

  //use system clock as source
  //note from the datasheet UARTCCC register description:
  //field is 0 (system clock) by default on reset
  //Good to be explicit in your code
  UART1_CC_R = 0x0;

  //re-enable UART1 and also enable RX, TX (three bits)
  //note from the datasheet UARTCTL register description:
  //RX and TX are enabled by default on reset
  //Good to be explicit in your code
  //Be careful to not clear RX and TX enable bits
  //(either preserve if already set or set them)
  UART1_CTL_R &= 0b1111110011111110;
  UART1_CTL_R |= 0b0000001100000001;



}
//Blocking call that sends 1 char over UART1
void uart_sendChar(char data){
	while(UART1_FR_R & 0x20){}

	//send data
	UART1_DR_R = data;
}
//Blocking call to receive over UART1
//returns char with data
char uart_receive(void){
	char data = 0;

	//wait to receive input
	while(UART1_FR_R & UART_FR_RXFE){}
	data = (char)(UART1_DR_R & 0xFF);
	return data;
	}

void uart_formattedStringToCybot(){
    int i = 0;
    char puttyChar;
    char strToReturn[20] = "                    "; //20 chars of whitespace
    for(i = 0; i < 20; i++){
        //recieve char input from putty
        puttyChar = uart_receive();
        uart_sendChar(puttyChar);
        //if they hit enter, break
        if(puttyChar == '\r'){
            uart_sendChar('\n');
            break;
        }


        strToReturn[i] = puttyChar;
        lcd_printf(strToReturn);

    }

    uart_sendStr("all done!\n\r");
}

void uart_sendStr(const char *data){
        //While not equal to null
        while(*data != '\0')
        {
            uart_sendChar(*data);
            data++;
        }
}
