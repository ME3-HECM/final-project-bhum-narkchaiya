#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

struct RGB_val {
    unsigned int L;
	unsigned int R;
	unsigned int G;
	unsigned int B;
};
/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Functions to read the L, red, green, and blue channels
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Luminosity(void);
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);

/********************************************//**
 *  Functions to read the red, green, and blue channels
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
void color_to_struct(struct RGB_values *rgb);
unsigned int color_process_easy(struct RGB_values *rgb);
unsigned int color_process_hard(struct RGB_values *rgb);
#endif
