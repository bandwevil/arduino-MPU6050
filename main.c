/*
 * CPE 329 - Spring 2013
 * Project 3: Motion Sensor
 *
 * Tyler Saadus and Jonathan Hernandez
 *
 * Sense motion via an MPU-6050 connected to an Arduino
 * Results are then transmitted over UART to be displayed on the PC
 *
 * Transmission format: a one byte identifier, then two bytes of data
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h> // Standard C library
#include "uart.h"
#include "twi.h"

void messageSend(char tag, int dataUp, int dataDown);
void nextRange(char reg);
void startSelfTest();
void initPCINT();
void changePower();

char newData;

/*
 * main: overall program flow control
 *
 * initializes UART, I2C interfaces for standard control, then loops while
 * transmitting current register values and looking for inputs.
 */
int main()
{
   int in = 0;
   unsigned char data[6];
   newData = 1;

   DDRC = 0xFF;   // Port C contains the pins for i2c
   DDRB = 1<<5;

   usart_init(9600, F_CPU );

   // set SCL to 400 kHz
   // SCL freq = (CPU freq)/(16 + 2*TWBR*4^TWPS)
   //          = 16*10^6/(16+2*3*4^1) = 400 kHz
   TWBR = 3;
   TWSR = 1;

   _delay_ms(1000);//Wait for power up and monitor connection

   usart_send('R'); //Send reset indicator
   if ((in = read_reg(0x75)) == 0x68) {
      uart_str("Found MPU!\n");
   } else {
      uart_str("init error, got value: ");
      usart_send(in);
      usart_send('\0'); //Send null byte to ensure monitor is killed
      return 0;
   }

   write_reg(0x6B, 0x02); //Disable sleep, use Y gyro for clocking

   while (1) {
      if (newData == 1) {
         read_reg_multiple(data, 0x3B, 6); //Read the accelerometer registers
         messageSend('x', data[0], data[1]);
         messageSend('y', data[2], data[3]);
         messageSend('z', data[3], data[4]);

         read_reg_multiple(data, 0x43, 6); //Read the gyroscope registers
         messageSend('r', data[0], data[1]);
         messageSend('p', data[2], data[3]);
         messageSend('Y', data[3], data[4]);
      }

      //parse control inputs
      while (usart_istheredata()) {
         switch (usart_recv()) {
            case 'a': //Change accelerometer full-scale range
               nextRange(0x1C);
               break;
            case 'g': //Change gyro full-scale range
               nextRange(0x1B);
               break;
            case 't': //Start self testing
               startSelfTest();
               break;
            case 'p': //Modify power setting
               changePower();
               break;
         }
      }
   }

   return 0;
}

/*
 * changePower: toggle between standard and low power interfaces
 * reads the current setting of cycle and writes based on that
 */
void changePower()
{
   if ((read_reg(0x6B) & 0x20) == 0) {
      write_reg(0x6B, 0x24); //Enable cycle, disable temp, use internal clock
      write_reg(0x6C, 0x37); //Wakeup of 40 Hz, disable gyroscopes
   } else {
      write_reg(0x6C, 0x00); //Enable all components
      write_reg(0x6B, 0x02); //Disable cycle, revert to Y gyro clock source
   }
}

/*
 * Run built-in self test on all accelerometers and gyros
 */
void startSelfTest()
{
   write_reg(0x1B, 0xE0);
   write_reg(0x1C, 0xF0);
}

/*
 * Switch the range setting of the gyro or accelerometer
 *
 * parameters:
 * reg - the register to modify, either 0x1B or 0x1C
 *       undefined operation for other values.
 */
void nextRange(char reg)
{
   char in;
   in = (read_reg(reg) & 0x18) >> 3;
   if (in != 3) {
      in++;
   } else {
      in = 0;
   }
   write_reg(reg, in << 3);
}

/*
 * Send the standard format of message over UART connection
 * message is 3 bytes long, one byte as a message identifier and then a 16 bit
 * number representing the data.
 */
void messageSend(char tag, int dataUp, int dataDown)
{
   usart_send(tag);
   usart_send(dataUp);
   usart_send(dataDown);
}
