#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>    // Standard C library

//reads data value from register with address regAdd
//return data value read on success, -1 on failure
int read_reg(int regAdd);

int read_reg_multiple(unsigned char* store, int regAdd, unsigned char count);

//writes data value to register with address regAdd
//return data value written on success, -1 on failure
int write_reg(int regAdd, int data);
