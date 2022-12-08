#include <xc.h>
#include "color.h"
#include "i2c.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    //turn on lights on buggy
    TRISHbits.TRISH1 = 0; //set H.LAMPS, pin RH1 as output
    TRISDbits.TRISD3 = 0; //set M.BEAMS, pin RD3 as output
    TRISDbits.TRISD4 = 0; //set BRAKE, pin RD4 as output
    TRISHbits.TRISH0 = 0; //set TURN R, pin RH0 as output
    TRISFbits.TRISF0 = 0; //set TURN L, pin RF0 as output
    
    LATHbits.LATH1 = 1; //first turn on the light
    LATDbits.LATD3 = 1; //set front LED to max brightness
    LATDbits.LATD4 = 1; //set back LED to max brightness
    LATHbits.LATH0 = 1; //set right signal LED
    LATFbits.LATF0 = 1; //set left signal LED 
}

void color_writetoaddr(char address, char value)
{
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Luminosity(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at L low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the L LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the L MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Green LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at Blue low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Blue LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}  

void color_read(struct RGB_val *rgb)
{
    rgb->L = color_read_Luminosity();
    __delay_ms(100);
    rgb->R = color_read_Red();
    __delay_ms(100);
    rgb->G = color_read_Green();
    __delay_ms(100);
    rgb->B = color_read_Blue();
}

/********************************************//**
    *color assignment
    1. red          turn right 90                           
    2. green        turn left 90                                                                         
    3. blue         turn 180                           
    4. yellow       rev 1, turn right 90                                    
    5. pink         rev 1, turn left 90                                  
    6. orange       turn right 135                                   
    7. light blue   turn left 135                                        
    8. white        finish (ET go home)                                    
 ***********************************************/

unsigned int color_processor_easy(struct RGB_val *rgb)
{
    unsigned int a = rgb->R;
    unsigned int b = rgb->G;
    unsigned int c = rgb->B;
    unsigned int color;
    if (a>=b & a>=c){color=1;} //if red strongest
    else if (b>=a & b>=c){color=2;} //if green strongest
    else {color=3;} //if blue strongest
    return color;
}

unsigned int color_processor_hard(struct RGB_val *rgb)
{
    unsigned int a = rgb->R;
    unsigned int b = rgb->G;
    unsigned int c = rgb->B;
    unsigned int color;
    if (a>=b & a>=c){color=1;} //if red strongest
    else if (b>=a & b>=c){color=2;} //if green strongest
    else {color=3;} //if blue strongest
    return color;
}

