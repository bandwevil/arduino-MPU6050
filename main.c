/*
 * CPE 329 - Spring 2013
 * Project 3: Motion Sensor
 *
 * Tyler Saadus and Jonathan Hernandez
 *
 * Sense motion via an MPU-6050 connected to an Arduino
 * Results are then transmitted over UART to be displayed on the PC
 *
 * main: Main functions and overall system control
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h> // Standard C library
#include "uart.h"
#include "twi.h"

void uart_str(char* out);
void messageSend(char tag, int dataUp, int dataDown);

int main()
{
   DDRB = 0xFF;
   DDRC = 0xFF;   // Port C contains the pins for i2c
   int in = 0, i;
   unsigned char data[6];

   usart_init(9600, F_CPU );

   // set SCL to 400 kHz
   // SCL freq = (CPU freq)/(16 + 2*TWBR*4^TWPS)
   //          = 16*10^6/(16+2*3*4^1) = 400 kHz
   TWBR = 3;
   TWSR = 1;

   _delay_ms(1500);//Wait for power up and monitor connection

   usart_send('R'); //Reset indicator
   if ((in = read_reg(0x75)) == 0x68) {
      uart_str("Found MPU!\n");
   } else {
      uart_str("init error, got value: ");
      usart_send(in);
      usart_send('\0');
      return 0;
   }

   write_reg(0x6B, 0x00); //Disable sleep, use Z gyro for clocking

   while (1) {
      i = 0;
      read_reg_multiple(data, 0x3B, 6); //Read the accelerometer registers
      messageSend('x', data[0], data[1]);
      messageSend('y', data[2], data[3]);
      messageSend('z', data[3], data[4]);

      read_reg_multiple(data, 0x43, 6); //Read the gyroscope registers
      messageSend('r', data[0], data[1]);
      messageSend('p', data[2], data[3]);
      messageSend('Y', data[3], data[4]);
   }

   return 0;
}

void messageSend(char tag, int dataUp, int dataDown)
{
   usart_send(tag);
   usart_send(dataUp);
   usart_send(dataDown);
}

void uart_str(char* out)
{
   int i = 0;

   while (out[i] != '\0') {
      usart_send(out[i]);
      i++;
   }
}
