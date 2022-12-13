#include <xc.h>
#include "color.h"
#include "i2c.h"
#include <stdbool.h>  
#include "serial.h"
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
    LATDbits.LATD4 = 0; //set back LED to max brightness
    LATHbits.LATH0 = 0; //set right signal LED
    LATFbits.LATF0 = 0; //set left signal LED 
    
    //setup pins for clicker board LEDs and buttons
    LATDbits.LATD7=0;   //set initial output state of D7 to off
    LATHbits.LATH3=0;   //set initial output state of H3 to on
    TRISDbits.TRISD7=0; //set TRIS value for pin D7 (output)
    TRISHbits.TRISH3=0; //set TRIS value for pin H3 (output)

    TRISFbits.TRISF2=1; //set TRIS value for pin F2 (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin F2  
    TRISFbits.TRISF3=1; //set TRIS value for pin F3 (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin F3
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

unsigned int color_processor_easy(struct RGB_val *rgb, unsigned int calibrated[])
{
    unsigned int l = rgb->L;
    unsigned int r = rgb->R;
    unsigned int g = rgb->G;
    unsigned int b = rgb->B;
    unsigned int color;  
    if (l<calibrated[28]+100 && l>calibrated[28]-100){
        return 8; //white card
    }
    if (r>g && r>b){//red or green
        if (g>b){return 2;}//green
        else{return 1;}//red
    }
    else{return 3;}//blue
    return color;
}

unsigned int color_processor_hard(struct RGB_val *rgb, unsigned int calibrated[])
{
    unsigned int l = rgb->L;
    unsigned int r = rgb->R;
    unsigned int g = rgb->G;
    unsigned int b = rgb->B;
    
    unsigned int maxl = calibrated[28];
    //compares recorded RGB values to calibrated values within a margin
    for (int i=0;i>4;i++){
        unsigned int cr = calibrated[4*i+1];
        unsigned int cg = calibrated[4*i+2];
        unsigned int cb = calibrated[4*i+3];
        //conditions to check if recorded value is within a margin "40"-wide of the calibrated values 
        bool condition_r = r>cr-10 && r<cr+10;
        bool condition_g = b>cb-10 && b<cb+10;
        bool condition_b = g>cg-10 && g<cg+10;
        if (l<0.5*maxl){
            if (condition_r && condition_g && condition_b){
                return i+1;
            }
        }
    }
//    for (int j=4;j>8;j++)
//        unsigned int cr2 = calibrated[4*j+1];
//        unsigned int cg2 = calibrated[4*j+2];
//        unsigned int cb2 = calibrated[4*j+3];
//        //conditions to check if recorded value is within a margin "40"-wide of the calibrated values 
//        bool condition_r2 = r>cr2-10 && r<cr2+10;
//        bool condition_g2 = b>cb2-10 && b<cb2+10;
//        bool condition_b2 = g>cg2-10 && g<cg2+10;
//        if (condition_r2 && condition_g2 && condition_b2){
//                return j+1;
//            }
//    }
}