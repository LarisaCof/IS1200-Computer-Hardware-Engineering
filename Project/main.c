/*
 * I2C Example project for the mcb32 toolchain
 * Demonstrates the temperature sensor and display of the Basic IO Shield
 * Make sure your Uno32-board has the correct jumper settings, as can be seen
 * in the rightmost part of this picture:
 * https://reference.digilentinc.com/_media/chipkit_uno32:jp6_jp8.png?w=300&tok=dcceb2
 */

#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "mipslab.h"

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200

/* Address of the temperature sensor on the I2C bus */
#define TEMP_SENSOR_ADDR 0x48

// Definitions for display_image()
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

/* Temperature sensor internal registers */
typedef enum TempSensorReg TempSensorReg;
enum TempSensorReg {
	TEMP_SENSOR_REG_TEMP,
	TEMP_SENSOR_REG_CONF,
	TEMP_SENSOR_REG_HYST,
	TEMP_SENSOR_REG_LIMIT,
};

// Global variables for the icons
int display_snowflake = 0;
int display_sun = 0;

// Timer
int pr_count = 0;
int sec_count = 0;
int min_count = 0;
int period = 1; // Period time, default value 1 minute

// Global temps
uint16_t highest = 0x0000; // Reset temperature value
uint16_t snowflake_or_sun = 0x1900; // Temperature value 10 celsius


char textbuffer[4][16];

const uint8_t const font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 2, 5, 2, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

// Sun icon
const uint8_t const sun [] = {
		~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x80, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x30, ~0x80, ~0x00, ~0x00,
		~0x00, ~0x80, ~0x18, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x80, ~0x00, ~0x00, ~0x00, ~0x00,
		~0x00, ~0x00, ~0x20, ~0x00, ~0x40, ~0x40, ~0x41, ~0x80, ~0x82, ~0x04, ~0x08, ~0x40, ~0x00, ~0x01, ~0x04, ~0x00,
		~0x00, ~0x07, ~0x00, ~0x10, ~0x20, ~0x08, ~0x00, ~0x04, ~0x02, ~0x00, ~0x01, ~0x00, ~0x80, ~0x80, ~0x00, ~0x00,
		~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x80, ~0x40, ~0x40, ~0x20, ~0x00, ~0x07, ~0x08, ~0x00, ~0x10, ~0x50, ~0x10,
		~0x10, ~0x00, ~0xC8, ~0x04, ~0x03, ~0x00, ~0x08, ~0x00, ~0x10, ~0x00, ~0x20, ~0x40, ~0x00, ~0x00, ~0x00, ~0x00,
		~0x00, ~0x00, ~0x00, ~0x00, ~0x01, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x10, ~0x0C, ~0x01, ~0x00, ~0x00,
		~0x00, ~0x00, ~0x00, ~0x0E, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00
};

// Snowflake icon
const uint8_t const snowflake [] = {
	~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x60, ~0xE0, ~0xC0, ~0xD8, ~0xF8, ~0x00, ~0x00, ~0x04, ~0x1C, ~0xF8, ~0xFE,
	~0x10, ~0x18, ~0x1C, ~0x04, ~0x00, ~0x00, ~0x00, ~0x60, ~0xF0, ~0xC0, ~0xE0, ~0x80, ~0x80, ~0x00, ~0x00, ~0x00,
	~0x40, ~0xD0, ~0xD8, ~0xF0, ~0xE1, ~0xC1, ~0xC1, ~0x81, ~0x81, ~0x83, ~0x87, ~0x8E, ~0x9C, ~0xB8, ~0xFF, ~0xFF,
	~0xC0, ~0xE0, ~0xE0, ~0xB0, ~0x98, ~0x9C, ~0x0E, ~0x07, ~0x03, ~0x01, ~0x01, ~0xC1, ~0xC0, ~0x60, ~0x60, ~0x40,
	~0x00, ~0x02, ~0x03, ~0x01, ~0x61, ~0xA0, ~0xE1, ~0xE1, ~0xF1, ~0x39, ~0x19, ~0x0D, ~0x0F, ~0x07, ~0xFF, ~0xFF,
	~0x01, ~0x03, ~0x07, ~0x0F, ~0x1D, ~0x19, ~0x31, ~0xF1, ~0xE3, ~0xC3, ~0x63, ~0xC7, ~0xCF, ~0x0B, ~0x03, ~0x00,
	~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x03, ~0x05, ~0x07, ~0x03, ~0x00, ~0x20, ~0x30, ~0x18, ~0x0C, ~0x7F, ~0x0F,
	~0x0C, ~0x38, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x07, ~0x01, ~0x03, ~0x03, ~0x00, ~0x01, ~0x00, ~0x00, ~0x00
};


// Get switches
int getsw(void){
    int sw = (PORTD >> 8) & 0x000F;  //Skifta bitarna 11-8 till plats till lsb, maska! Spara som int.

    return sw;
}

// Get buttons
int getbtns(void){
    int btn = (PORTD >> 5) & 0x007;    //Skifta bitarna 7-4 till plats till lsb, maska! Spara som int.

    return btn;
}

// Function copied from lab3
/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}

void delay(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

void display_init() {
	DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	delay(10);
	DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	delay(1000000);

	spi_send_recv(0xAE);
	DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	delay(10);
	DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	delay(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	delay(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_image(int x, const uint8_t *data) {
    int i, j;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(x & 0xF);
        spi_send_recv(0x10 | ((x >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 32; j++)
            spi_send_recv(~data[i*32 + j]);
    }
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_update() {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;

		// Original value j < 16 but j < 12 removes flimmering on the icon
		for(j = 0; j < 12; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Wait for I2C bus to become idle */
void i2c_idle() {
	while(I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //TRSTAT
}

/* Send one byte on I2C bus, return ack/nack status of transaction */
bool i2c_send(uint8_t data) {
	i2c_idle();
	I2C1TRN = data;
	i2c_idle();
	return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

/* Receive one byte from I2C bus */
uint8_t i2c_recv() {
	i2c_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1
	i2c_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0
	return I2C1RCV;
}

/* Send acknowledge conditon on the bus */
void i2c_ack() {
	i2c_idle();
	I2C1CONCLR = 1 << 5; //ACKDT = 0
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack() {
	i2c_idle();
	I2C1CONSET = 1 << 5; //ACKDT = 1
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send start conditon on the bus */
void i2c_start() {
	i2c_idle();
	I2C1CONSET = 1 << 0; //SEN
	i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart() {
	i2c_idle();
	I2C1CONSET = 1 << 1; //RSEN
	i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop() {
	i2c_idle();
	I2C1CONSET = 1 << 2; //PEN
	i2c_idle();
}

/* Convert 8.8 bit fixed point to string representation*/
char *fixed_to_string(uint16_t num, char *buf) {
	bool neg = false;
	uint32_t n;
	char *tmp;

	if(num & 0x8000) {
		num = ~num + 1;
		neg = true;
	}

	if(min_count < period)
	{
		if(num > highest)
			highest = num;
	}

	// Changing picture depending on the temperature
	if (num >= snowflake_or_sun)
        display_sun = 1;
	else
		display_snowflake = 1;

	// Checking the switches
	int switch_value = getsw();

	// Depending on the positions the temperature will be converted to celsius, kelvin or fahrenheit
	if (switch_value == 0)  // Celsius default unit
		n = (num >> 8);
	else if (switch_value == 8) // Farenheit switch 3
		n = (((num >> 8) * 2) + 32);

	buf += 4;
	tmp = buf;
	do {
		*--tmp = (n  % 10) + '0';
		n /= 10;
	} while(n);
	if(neg)
		*--tmp = '-';

	n = num;
	if(!(n & 0xFF)) {
		*buf = 0;
		return tmp;
	}
	*buf++ = '.';
	while((n &= 0xFF)) {
		n *= 10;
		*buf++ = (n >> 8) + '0';
	}
	*buf = 0;

	return tmp;
}

uint32_t strlen(char *str) {
	uint32_t n = 0;
	while(*str++)
		n++;
	return n;
}

void display_temperature(char *s)
{
	display_string(0, "Temperature:");
	display_string(1, s);
	display_string(2, "");
	display_string(3, "");
}

void display_timer(void)
{
	display_string(0, "s, min:");
	display_string(1, itoaconv(sec_count));
	display_string(2, itoaconv(min_count));
}

void display_period_and_highest_temp(void)
{
	// Checking the buttons
    int buttons = getbtns();


    unsigned char* temp = &highest;
    int first = *temp;  //Point, unused for now
    int second = *(temp+1); //Integer

	display_string(0 , "Period(min):");
	display_string(1, itoaconv(period));
	display_string(2, "Highest temp");
	display_string(3, itoaconv(second));

    if (buttons == 1) // Button 2
        period++;
    else if (buttons == 2) // Button 3
        period--;
}

void display_error_message(void)
{
	display_string(0, "Invalid");
	display_string(1, "switch");
	display_string(2, "combination!");
	display_string(3, "");
}

void display_image_sun(void)
{
	display_image(96, sun);
    display_sun = 0;
}

void display_image_snowflake(void)
{
	display_image(96, snowflake);
    display_snowflake = 0;
}

int main(void) {

	uint16_t temp;
	char buf[32], *s, *t;

	T2CON = 0; // Stop timer and clear control register
	T2CONSET = 0x50; // Prescale 1:32
	TMR2 = 0; // Clear timer register
	PR2 = 80000000 / 32 / 100; // Load period register
	T2CONSET = 0x8000; // Start timer

	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;

	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
    SPI2CON |= 0x60;

	/* Turn on SPI */
	SPI2CONSET = 0x8000;

	/* Set up i2c */
	I2C1CON = 0x0;
	/* I2C Baud rate should be less than 400 kHz, is generated by dividing
	the 40 MHz peripheral bus clock down */
	I2C1BRG = 0x0C2;
	I2C1STAT = 0x0;
	I2C1CONSET = 1 << 13; //SIDL = 1
	I2C1CONSET = 1 << 15; // ON = 1
	temp = I2C1RCV; //Clear receive buffer

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	display_init();
	display_string(0, "");
	display_string(1, "");
	display_string(2, "");
	display_string(3, "");
	display_update();

	/* Send start condition and address of the temperature sensor with
	write mode (lowest bit = 0) until the temperature sensor sends
	acknowledge condition */
	do {
		i2c_start();
	} while(!i2c_send(TEMP_SENSOR_ADDR << 1));

	/* Send register number we want to access */
	i2c_send(TEMP_SENSOR_REG_CONF);
	/* Set the config register to 0 */
	i2c_send(0x0);
	/* Send stop condition */
	i2c_stop();

	for(;;) {
		/* Send start condition and address of the temperature sensor with
		write flag (lowest bit = 0) until the temperature sensor sends
		acknowledge condition */
		do {
			i2c_start();
		} while(!i2c_send(TEMP_SENSOR_ADDR << 1));

		/* Send register number we want to access */
		i2c_send(TEMP_SENSOR_REG_TEMP);

		/* Now send another start condition and address of the temperature sensor with
		read mode (lowest bit = 1) until the temperature sensor sends
		acknowledge condition */
		do {
			i2c_start();
		} while(!i2c_send((TEMP_SENSOR_ADDR << 1) | 1));

		/* Now we can start receiving data from the sensor data register */
		temp = i2c_recv() << 8;
		i2c_ack();
		temp |= i2c_recv();
		/* To stop receiving, send nack and stop */
		i2c_nack();
		i2c_stop();

		s = fixed_to_string(temp, buf);
		t = s + strlen(s);

		// Check interrupt flag
		if (IFS(0) & 0x100)
		{
			pr_count++;
			IFSCLR(0) = 0x100;
		}

		if (pr_count == 10)
		{
			sec_count++;
			pr_count = 0;
		}

		if (sec_count == 60)
		{
			min_count++;
			sec_count = 0;
			if (min_count == period)
				min_count = 0;
				highest = 0x0000;
		}

		// Checking the switches
		int switch_value = getsw();

		*t++ = ' ';

		// Depending on the positions the temperature will be displayed in celsius, kelvin or fahrenheit
		if (switch_value == 0)
		{
			*t++ = 7;
			*t++ = 'C';
			*t++ = 0;

			display_temperature(s);
		}

		else if (switch_value == 8)
		{
			*t++ = 7;
			*t++ = 'F';
			*t++ = 0;

			display_temperature(s);
		}
		else if(switch_value == 2)
			display_timer();
		else if (switch_value == 1)
			display_period_and_highest_temp();
		// If two or more switches are switched an error message will appear
		else
			display_error_message();

		// Displaying icon depending on the temperature
		if (display_sun)
			display_image_sun();
		else if (display_snowflake)
			display_image_snowflake();

		display_update();
		delay(1000000);
	}

	return 0;
}
