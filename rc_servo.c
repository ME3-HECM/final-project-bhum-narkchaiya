#include <xc.h>
#include "rc_servo.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * individual interrupt sources are enabled in their respective module init functions
************************************/
void Interrupts_init(void)
{
    // turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    INTCONbits.IPEN = 1; //enable priority level on interrupts
    INTCONbits.PEIE = 1;
    
    
    PIE0bits.TMR0IE = 1; //enable time interrupt
    IPR0bits.TMR0IP = 1; //set as high priority
    PIR0bits.TMR0IF = 0; //interrupt initially set as off
    
    PIE2bits.C1IE=1; //enable interrupt source Comparator 1
    IPR2bits.C1IP=0; //set as low priority
    PIR2bits.C1IF=0; //interrupt initially set as off
    
    INTCONbits.GIE = 1; //enable global interrupt
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if(PIR0bits.TMR0IF||!PORTFbits.RF2){ //check the Timer0 interrupt source
        //LATHbits.LATH3 = !LATHbits.LATH3;  //toggle the LED
        
        time++; //counts time in seconds
        
        //count from 3035 to 62535 to toggle LED H3 every second (timer runs at 62.5 kHz)
        TMR0L = 0b11011011;
        TMR0H = 0b00001011;
        
        PIR0bits.TMR0IF=0; //clear the interrupt flag
	}
//    if (PIR0bits.TMR0IF)
//    {
//        if(LATEbits.LATE2){ //if output pin currently high
//            write16bitTMR0val(65535-off_period); //set new off_period
//            LATEbits.LATE2=0; //turn your output pin off here
//            //LATDbits.LATD7 = !LATDbits.LATD7;
//        } else {
//            write16bitTMR0val(65535-on_period);  //set new on_period
//            LATEbits.LATE2=1; //turn your output pin off here
//            //LATHbits.LATH3 = !LATHbits.LATH3;
//        }
//    }
//    PIR0bits.TMR0IF=0; 
}

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0111; // need to work out prescaler to produce a timer tick corresponding to 1 deg angle change
    //prescalar 128
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // it's a good idea to initialise the timer so that it initially overflows after 20 ms
    TMR0H=(65535-T_PERIOD)>>8;            
    TMR0L=(unsigned char)(65535-T_PERIOD); // casting to unsigned char here to suppress warning
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to write a full 16bit timer value
 * Note TMR0L and TMR0H must be written in the correct order, or TMR0H will not contain the correct value
************************************/
void write16bitTMR0val(unsigned int tmp)
{
    TMR0H=tmp>>8; //MSB written
    TMR0L=tmp; //LSB written and timer updated
}

/************************************
 * Function to set the on_period and off_period variables used in the ISR
 * the on_period varies linearly according to angle (-90 deg is 0.5 ms, +90 is 2.1 ms)
 * off_period is the remaining time left (calculate from on_period and T_PERIOD)
************************************/
void angle2PWM(int angle){
    on_period = angle * (200/180) + 162;	//avoid floating point numbers and be careful of calculation order...
    off_period = T_PERIOD-on_period;
}

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/

