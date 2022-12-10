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
    Timer0_init(); //timer interrupt
    I2C_2_Master_Init(); //serial communication
    initUSART4(); //serial communication
    
    
    //declare variables
    unsigned int color_flag; //interrupt when card is in front of buggy
    unsigned int color_name;
    unsigned int color_storage[50];
    unsigned int time_storage[50];
    unsigned int home = 0;
    struct RGB_val RGB_calibrated;
    struct RGB_val RGB_recorded;
    
    //calibration variables
    unsigned int color_calibrated[24];
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
    
    //serial communication for color readout
    //on RealTerm: Baud=9600, Port 3
    color_read(&RGB_calibrated);
    char readout[50];
    sprintf(readout,"%d %d %d %d \r\n", RGB_calibrated.L,RGB_calibrated.R,RGB_calibrated.G,RGB_calibrated.B);
    sendStringSerial4(readout);

    while (1) {
//        //calibration
//        while (PORTFbits.RF3); //wait for button press on F3
//        int i_calibrate;
//        for (i=0;i<9;i++){//add to array 3 rgb values for every color = 24 elements in array
//            color_read(&RGB_calibrated);
//        }
        
        //mode selection
        while (PORTFbits.RF3 &  PORTFbits.RF2); //wait for any button press
        if (!PORTFbits.RF2){LATDbits.LATD7 = !LATDbits.LATD7;} //F2 button (easy mode)
        else if (!PORTFbits.RF3){LATHbits.LATH3 = !LATHbits.LATH3;} //F3 button (hard mode)
        
        //maze time
        int i_storage; //iterator for storing path of buggy
        while (!home) {
            if (LATDbits.LATD7){ //easy mode
                if (color_flag){
                    color_read(&RGB_recorded); //read RGB values of card
                    color_name = color_processor_easy(&RGB_recorded); //color detection for red, green, blue, white only
                    //color_record(color_name); //records current navigation
                    //store to array
                    
                    if (color_name != 8) { //moves onto return home phase once reaching the white card
                        color_flag = 0; 
                        home = 1; //stops while loop and moves onto the return home phase
                    }
                }
            }
            else if (LATHbits.LATH3) { //hard mode
                //color detection for harder colors
                color_read(&RGB_recorded);
            }
            __delay_ms(200); // call built in delay function 
        }
        //return home algorithm
        int j;
        size_t n = sizeof(color_storage)/sizeof(color_storage[0]); //Mark Harrison @StackOverflow (2008)
        for (j=0;j<n;j++){
            //motor_action(&color_storage, &time_storage);
        }
            
        
    }
    
    //reach destination code
    //if color = white
    //play back recorded colors from array
    
    
    while (1) {
        //turn_on_color();
        Sleep();
    }
}