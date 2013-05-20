#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>    // Standard C library

/*
 * Read data from the provided register
 * Convenience wrapper around read_reg_multiple() for one byte
 *
 * parameters:
 * regAdd - the register to read from
 *
 * returns:
 * A single byte of the read data on success, < 0 on failure
 */
int read_reg(int regAdd);

/*
 * Read a chunk of data over S2I into the provided array
 *
 * parameters:
 * store - Array of characters to read into, must at least as large as count
 * regAdd - The register to start reading from
 * count - number of registers to read, undefined operation for 0 values
 *
 * returns:
 * 0 on success or less than 0 on failure
 */
int read_reg_multiple(unsigned char* store, int regAdd, unsigned char count);

/*
 * Write a byte of data to a register over I2C
 *
 * parameters:
 * regAdd - the register to write to
 * data - the byte to write
 *
 * returns:
 * data on success, less than 0 on failure
 */
int write_reg(int regAdd, int data);
