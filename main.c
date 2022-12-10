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
    //initialize functions
    color_click_init(); //color board
    initDCmotorsPWM(20); //DC motor at 20 ms PWM period
    Interrupts_init(); //light sensor interrupt
    Timer0_init(); //timer interrupt
    I2C_2_Master_Init(); //serial communication
    initUSART4(); //serial communication
    
    //
    struct DC_motor motorL, motorR; //create structures for both sides motors
    unsigned char PWMcycle = 200; //set PWMcycle for motor
    motorL.power = 0; //initial motor power
    motorL.direction = 0; //initial direction : reverse
    motorL.dutyHighByte = (unsigned char *)(&PWM6DCH); //set channel
    motorL.dir_LAT = (unsigned char *)(&LATE); //set register
    motorL.dir_pin = 4; //set pin
    motorL.PWMperiod = PWMcycle; //set PWMcycle
    motorR.power = 0;//initial motor power
    motorR.direction = 0; //initial direction : reverse
    motorR.dutyHighByte = (unsigned char *)(&PWM7DCH); //set channel
    motorR.dir_LAT = (unsigned char *)(&LATG); //set register
    motorR.dir_pin = 6; //set Pin
    motorR.PWMperiod = PWMcycle; //set PWM cycle
    setMotorPWM(&motorL);
    setMotorPWM(&motorR);
    
    //color variables
    unsigned int color_flag; //interrupt when card is in front of buggy
    unsigned int color_name;
    unsigned int color_path[15];
    struct RGB_val RGB_recorded;
    
    unsigned int time_path[15];
    unsigned int time;
    unsigned int time_flag; //timer interrupt flag
    int j; //iterator for storing path of buggy
    unsigned int home = 0;
    
    unsigned int color_calibrated[24];
    struct RGB_val RGB_calibrated;
    
    //serial communication for color readout on RealTerm: Baud=9600, Port 3
    color_read(&RGB_calibrated);
    char readout[50];
    sprintf(readout,"%d %d %d %d \r\n", RGB_calibrated.L,RGB_calibrated.R,RGB_calibrated.G,RGB_calibrated.B);
    sendStringSerial4(readout);

    while (1) {
//        //calibration
//        while (PORTFbits.RF3); //wait for button press on F3
//        for (int i=0;i<9;i++){//add to array 3 rgb values for every color = 24 elements in array
//            color_read(&RGB_calibrated);
//        }
        
        //timer
        if (time_flag==1){
            time++;
            time_flag = 0;
        }
        
        //mode selection
        while (PORTFbits.RF3 &  PORTFbits.RF2); //wait for any button press
        if (!PORTFbits.RF2){LATDbits.LATD7 = 1;} //F2 button (easy mode)
        else {LATHbits.LATH3 = 1;} //F3 button (hard mode)
        
        //maze time
        while (!home) {
            if (color_flag){
                stop(&motorL,&motorR); //stop the buggy
                color_read(&RGB_recorded); //read RGB values of card
                if (LATDbits.LATD7){color_name = color_processor_easy(&RGB_recorded);} //color detection for easy mode
                else {color_name = color_processor_hard(&RGB_recorded);} //color detection for hard mode 
                color_path[j] = color_name; //store color to array
                time_path[j] = time; //store time taken from the last action to array
                motor_action(color_name,&motorL,&motorR); //perform action depending on the color of the card
                j++;
                time = 0; //reset clock
                //moves onto return home phase once reaching the white card
                if (color_name != 8) { 
                    color_flag = 0; //clear flag
                    home = 1; //stops while loop
                }
            }
            else if (LATHbits.LATH3) { //hard mode
                //color detection for harder colors
                color_read(&RGB_recorded);
            }
            __delay_ms(200); // call built in delay function 
        }
        
        //return home
        for (int k=0;k<15;k++){
            motor_action(color_path[k],&motorL,&motorR); //need to write opposite functions
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