#include "uart.h"


/*
 * Initialize the UART communications for the given baud and speed
 */
void usart_init(uint16_t baudin, uint32_t clk_speedin)
{
   uint32_t ubrr = (clk_speedin/16UL)/baudin-1;
   UBRR0H = (unsigned char)(ubrr>>8);
   UBRR0L = (unsigned char)ubrr;
   /* Enable receiver and transmitter */
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);
   /* Set frame format: 8data, 1stop bit */
   UCSR0C = (1<<USBS0)|(3<<UCSZ00);
   UCSR0A &= ~(1<<U2X0);
}

/*
 * Send an array of characters over UART
 * this is simply a wrapper that repeats usart_send() as needed
 */
void uart_str(char* out)
{
   int i = 0;

   while (out[i] != '\0') {
      usart_send(out[i]);
      i++;
   }
}

/*
 * Send a single byte of data over UART
 */
void usart_send( uint8_t data )
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR0A & (1<<UDRE0)) );
   /* Put data into buffer, sends the data */
   UDR0 = data;
}

/* the receive data function. Note that this a blocking call
   Therefore you may not get control back after this is called 
   until a much later time. It may be helpfull to use the 
   istheredata() function to check before calling this function
   @return 8bit data packet from sender
   */
uint8_t  usart_recv(void)
{
   /* Wait for data to be received */
   while ( !(UCSR0A & (1<<RXC0)) )
      ;
   /* Get and return received data from buffer */
   return UDR0;
}

/* function check to see if there is data to be received
   @return true is there is data ready to be read */
uint8_t  usart_istheredata(void)
{
   return (UCSR0A & (1<<RXC0));
}
