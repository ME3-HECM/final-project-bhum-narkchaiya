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
#include <stdio.h>

#define _XTAL_FREQ 64000000

void main(void) {
    // setup pin for output (connected to LED)
    LATDbits.LATD7=0;   //set initial output state of D7 to off
    LATHbits.LATH3=1;   //set initial output state of H3 to on
    TRISDbits.TRISD7=0; //set TRIS value for pin D7 (output)
    TRISHbits.TRISH3=0; //set TRIS value for pin H3 (output)

    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin F2 (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin F2  
    
    //initialise functions
    color_click_init(); //initiate color board
    
    while (1) {
        
        //------------mode selector---------------
        while (PORTFbits.RF3 &  PORTFbits.RF2); //empty while loop (wait for button press)
        if (!PORTFbits.RF2){ //wait for F2 button press to choose easy mode
            LATDbits.LATD7 = !LATDbits.LATD7; //toggle LED D7
        }
        else if (!PORTFbits.RF3){ //wait for F3 button press to choose hard mode
            LATHbits.LATH3 = !LATHbits.LATH3; //toggle LED H3
        }
        //----------------------------------------
        
        while (1) {
            if (LATDbits.LATD7){ //easy mode
                //color detection for red, green, blue, white only
            }
            else if (LATHbits.LATH3) {
                //color detection for harder colors
            }
        }
        __delay_ms(200); // call built in delay function 
    }
    
    //reach destination code
    //if color = white
    
    while (1) {
        turn_on_color();
        Sleep();
    }
}