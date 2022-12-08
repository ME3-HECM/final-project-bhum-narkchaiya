// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

//import all header files here
#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "rc_servo.h"
#include "serial.h"
#include <stdio.h>

#define _XTAL_FREQ 64000000

void main(void) {
    // setup pin for output (connected to LED)
    LATDbits.LATD7=0;   //set initial output state of D7 to off
    LATHbits.LATH3=0;   //set initial output state of H3 to on
    TRISDbits.TRISD7=0; //set TRIS value for pin D7 (output)
    TRISHbits.TRISH3=0; //set TRIS value for pin H3 (output)

    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin F2 (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin F2  
    TRISFbits.TRISF3=1; //set TRIS value for pin F3 (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin F3
    
    //initialize functions
    color_click_init(); //color board
    initDCmotorsPWM(20); //DC motor at 20 ms PWM period
    Interrupts_init(); //light sensor interrupt
    Timer0_Init(); //timer interrupt
    I2C_2_Master_Init(); //serial communication
    initUSART4(); //serial communication
    
    
    //declare variables
    extern unsigned int flag_color; //interrupt when card is in front of buggy
    unsigned int color;
    struct RGB_val asdf;
    
    //serial communication for color readout
    //color = read_color_function();
    //char readout[50];
    //sprintf(readout,"%d \r\n", color.R, color.G, color.B);
    //sendStringSerial4(readout);

    while (1) {
        
        //------------mode selector---------------
        while (PORTFbits.RF3 &  PORTFbits.RF2); //wait for button press
        if (!PORTFbits.RF2){LATDbits.LATD7 = !LATDbits.LATD7;} //F2 button (easy mode)
        else if (!PORTFbits.RF3){LATHbits.LATH3 = !LATHbits.LATH3;} //F3 button (hard mode)
        //----------------------------------------
        
        while (1) {
            if (LATDbits.LATD7){ //easy mode
                if (flag_color){
                    color_to_struct(&asdf); //read RGB values of card
                    color = color_process_easy(&asdf); //color detection for red, green, blue, white only
                    
                }
            }
            else if (LATHbits.LATH3) { //hard mode
                //color detection for harder colors
                color_to_struct(&asdf);
            }
            __delay_ms(200); // call built in delay function 
        }
        
    }
    
    //reach destination code
    //if color = white
    
    while (1) {
        //turn_on_color();
        Sleep();
    }
}