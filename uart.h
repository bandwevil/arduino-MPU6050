#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>     // Standard C library

#define BAUD_PRESCALE 103

/*
 * Initialize the UART communications for the given baud and speed
 */
void usart_init(uint16_t baudin, uint32_t clk_speedin);

/*
 * Send a single byte of data over UART
 */
void usart_send( uint8_t data );

/*
 * Send an array of characters over UART
 * this is simply a wrapper that repeats usart_send() as needed
 */
void uart_str(char* out);

/*
 * Receive a byte over UART
 * Blocks until input is actually received, so checking istheredata() first
 * might be a good idea
 */
uint8_t  usart_recv(void);

/*
 * Check if there is data waiting to be read on the UART signal
 */
uint8_t  usart_istheredata(void);
