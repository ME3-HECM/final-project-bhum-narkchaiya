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
    unsigned char PWMcycle = 199; //set PWMcycle for motor
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
    
    //declare variables
    unsigned int color_flag;
    unsigned int color_name = 0;
    unsigned int color_path[5] = {1,0,2,0,3};
    int color_calibrated[32];
    unsigned int lowerbound_calibrated;
    unsigned int upperbound_calibrated;
    struct RGB_val RGB_calibrated;
    struct RGB_val L_calibrated;
    struct RGB_val RGB_recorded;
    unsigned int time_path[5] = {1,1,1,1,1};
    unsigned int time_return;
    int j; 

    //motor testing
//    while (1) {
//        motor_action(2,&motorL,&motorR);
//        __delay_ms(2000);
//        motor_action_return(2,&motorL,&motorR);
//        __delay_ms(500);
//    }
    while (1) {  
        //calibration for detecting color of the card
        while (PORTFbits.RF3); //wait for button press on F3
        for (int i=0;i<8;i++){//add to array 4 (lrgb) values for every color = 32 elements in array
            LATHbits.LATH3 = 1; //turn on indicator light on H3 LED
            color_read(&RGB_calibrated);
            color_calibrated[4*i] = RGB_calibrated.L;
            color_calibrated[4*i+1] = RGB_calibrated.R;
            color_calibrated[4*i+2] = RGB_calibrated.G;
            color_calibrated[4*i+3] = RGB_calibrated.B;     
            __delay_ms(1500);
            LATHbits.LATH3 = 0; //turn off indicator light on H3 LED
            __delay_ms(1000);
        }
        for (int i=0;i<8;i++){
            char readout3[100];
            sprintf(readout3,"%d %d %d %d %d \r\n", i+1,color_calibrated[4*i],color_calibrated[4*i+1],color_calibrated[4*i+2],color_calibrated[4*i+3]);
            sendStringSerial4(readout3); //serial communication
            __delay_ms(500);
        }
        //setting bounds for detecting if the card is in front of the buggy
        lowerbound_calibrated = color_calibrated[8]*0.95; //L value of blue card (minimum)
        upperbound_calibrated = color_calibrated[28]; //L value of white card (maximum)
        
        //mode selection
        while (PORTFbits.RF3 &  PORTFbits.RF2); //wait for any button press
        if (!PORTFbits.RF2){LATDbits.LATD7 = 1;} //F2 button (easy mode)
        else {LATHbits.LATH3 = 1;} //F3 button (hard mode)

        //color testing
        char test0[100];
        sprintf(test0,"TESTING \r\n");
        sendStringSerial4(test0); //serial communication
        while (1){
            color_read(&RGB_recorded);
            __delay_ms(50);
            color_name = color_processor_easy(&RGB_recorded,color_calibrated);
            __delay_ms(50);
            char test[100];
            sprintf(test,"%d \r\n", color_name);
            sendStringSerial4(test); //serial communication        
        }
        
//        //motor tests  
//        motor_action(1,&motorL,&motorR); //red
//        __delay_ms(2000);
//        motor_action(2,&motorL,&motorR); //green
//        __delay_ms(2000);
//        motor_action(3,&motorL,&motorR); //blue
//        __delay_ms(2000);
//        motor_action(4,&motorL,&motorR); //yellow
//        __delay_ms(2000);
//        motor_action(5,&motorL,&motorR); //pink
//        __delay_ms(2000);
//        motor_action(6,&motorL,&motorR); //orange
//        __delay_ms(2000);
//        motor_action(7,&motorL,&motorR); //light blue
//        __delay_ms(2000);
//        stop(&motorL,&motorR);
//        __delay_ms(2000);
//        __delay_ms(2000);
//        __delay_ms(2000);
//        __delay_ms(2000);
//        __delay_ms(2000);
//        
        //return home test preprogrammed
        for (int k=5;k>0;k--){
            INTCONbits.GIE = 0; //turn off global interrupts
            time_return = 0; //reset clock
            motor_action_return(color_path[k],&motorL,&motorR); //need to write opposite functions
            __delay_ms(500);
            reverse_onesquare(&motorL,&motorR);
            while (time_return < time_path[k]) { 
                __delay_ms(500);
                time_return++; 
                stop(&motorL,&motorR);
                __delay_ms(100);
            }
            stop(&motorL,&motorR);        
        }   
        
      
        //maze 
        while (color_name != 8){
            time = 0; //reset clock
            motor_action(0,&motorL,&motorR);
            __delay_ms(500);
            color_read(&RGB_recorded);
            if (RGB_recorded.L<upperbound_calibrated && RGB_recorded.L>lowerbound_calibrated){color_flag=1;} //raise flag if there is a card in front
            if (color_flag){
                LATHbits.LATH0 = 1; //set right signal LED
                LATFbits.LATF0 = 1; //set left signal LED 
                stop(&motorL,&motorR); //stop the buggy
                color_read(&RGB_recorded); //read RGB values of card
                if (LATDbits.LATD7)
                {color_name = color_processor_easy(&RGB_recorded,color_calibrated);} //color detection for easy mode
                else 
                {color_name = color_processor_hard(&RGB_recorded,color_calibrated);} //color detection for hard mode 
                color_path[j] = color_name; //store color to array
                time_path[j] = time; //store time taken from the last action to array
                motor_action(color_name,&motorL,&motorR); //perform action depending on the color of the card
                
                j++;
                time = 0; //reset clock
                color_flag = 0; //clear flag
                LATHbits.LATH0 = 0; //set right signal LED
                LATFbits.LATF0 = 0; //set left signal LED
            }
            __delay_ms(200); // call built in delay function 
        }
        
        //return home
        spin_180(&motorL,&motorR);
        for (int m=0;m>12;m++){
            LATHbits.LATH3=!LATHbits.LATH3;
        }
        for (int k=15;k>0;k--){
            INTCONbits.GIE = 0; //turn off global interrupts
            time_return = 0; //reset clock
            motor_action_return(color_path[k],&motorL,&motorR); //need to write opposite functions
            forward(&motorL,&motorR);
            while (time_return < time_path[k]) { 
                __delay_ms(500);
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