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
    initUSART4(); //serial communication Baud=9600, Port 3

    //motor initialisation
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
    unsigned int color_name;
    unsigned int color_path[15];
    int color_calibrated[32];
    struct RGB_val RGB_calibrated;
    struct RGB_val L_calibrated;
    struct RGB_val RGB_recorded;
    
    //path storage variables
    unsigned int time_path[15];
    unsigned int time_return;
    int j; //iterator

    while (1) {
        //calibration for stopping at a card
        color_read(&L_calibrated);    
        char readout1[100];
        sprintf(readout1,"%d %d %d %d \r\n", RGB_calibrated.L,RGB_calibrated.R,RGB_calibrated.G,RGB_calibrated.B);
        sendStringSerial4(readout1);
        __delay_ms(500);
        
        //calibration for detecting color of the card
        while (PORTFbits.RF3); //wait for button press on F3
        for (int i=0;i<8;i++){//add to array 4 (lrgb) values for every color = 32 elements in array
            color_read(&RGB_calibrated);
            color_calibrated[4*i] = RGB_calibrated.L;
            color_calibrated[4*i+1] = RGB_calibrated.R;
            color_calibrated[4*i+2] = RGB_calibrated.G;
            color_calibrated[4*i+3] = RGB_calibrated.B;
            LATHbits.LATH3 = 1; //turn on indicator light on H3 LED
            
            char readout2[100];
            sprintf(readout2,"%d %d %d %d \r\n", RGB_calibrated.L,RGB_calibrated.R,RGB_calibrated.G,RGB_calibrated.B);
            sendStringSerial4(readout2); //serial communication
            __delay_ms(2000);
            LATHbits.LATH3 = 0; //turn off indicator light on H3 LED
            __delay_ms(2000);
        }
        
        //mode selection
        while (PORTFbits.RF3 &  PORTFbits.RF2); //wait for any button press
        if (!PORTFbits.RF2){LATDbits.LATD7 = 1;} //F2 button (easy mode)
        else {LATHbits.LATH3 = 1;} //F3 button (hard mode)
        
        //maze time
        while (color_name != 8){
            time = 0; //reset clock
            forward(&motorL,&motorR);
            color_read(&RGB_recorded);
            if (&RGB_recorded.L>color_calibrated[8]){
                stop(&motorL,&motorR); //stop the buggy
                color_read(&RGB_recorded); //read RGB values of card
                if (LATDbits.LATD7){color_name = color_processor_easy(&RGB_recorded);} //color detection for easy mode
                else {color_name = color_processor_hard(&RGB_recorded,color_calibrated);} //color detection for hard mode 
                color_path[j] = color_name; //store color to array
                time_path[j] = time; //store time taken from the last action to array
                
                //serial communication
                char readout3[100];
                sprintf(readout3,"%d %d %d %d %d \r\n", color_name,RGB_recorded.L,RGB_recorded.R,RGB_recorded.G,RGB_recorded.B);
                sendStringSerial4(readout3);
            
                motor_action(color_name,&motorL,&motorR); //perform action depending on the color of the card
                
                j++;
                time = 0; //reset clock
                color_flag = 0; //clear flag
            }
            __delay_ms(200); // call built in delay function 
        }
        
        //return home
        spin_180(&motorL,&motorR);
        for (int k=0;k<15;k++){
            INTCONbits.GIE = 0; //turn off global interrupts
            time_return = 0; //reset clock
            motor_action(color_path[k],&motorL,&motorR); //need to write opposite functions
            forward(&motorL,&motorR);
            while (time_return < time_path[k]) { 
                __delay_ms(50);
                time_return++; 
            }
            stop(&motorL,&motorR);        
        }   
    }
    
    //sleepy time
    while (1) {
        Sleep();
    }
}