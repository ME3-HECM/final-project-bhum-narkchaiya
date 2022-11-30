#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and PWM for DC motor control
void initDCmotorsPWM(int PWMperiod){
	//initialise your TRIS and LAT registers for PWM
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
    
    LATEbits.LATE2 = 0;
    LATEbits.LATE4 = 0;
    LATCbits.LATC7 = 0;
    LATGbits.LATG6 = 0;
    // timer 2 config
    T2CONbits.CKPS=0b0011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    //Tpwn * (64000000/4*8) - 1
    T2PR=PWMperiod * 2000000 - 1; //Period reg 10kHz base period, for 10kHz, the time slot is 1*10^-4, prescaler should be smaller than this
    T2CONbits.ON=1;
    
    RE2PPS=0x0A; //PWM6 on RE2
    RC7PPS=0x0B; //PMW7 on RC7

    PWM6DCH=0; //0% power
    PWM7DCH=0; //0% power
    
    PWM6CONbits.EN = 1;
    PWM7CONbits.EN = 1;
}


// function to set PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
	int PWMduty; //tmp variable to store PWM duty cycle

	if (m->direction){ //if forward
		// low time increases with power
		PWMduty=m->PWMperiod - ((int)(m->power)*(m->PWMperiod))/100;
	}
	else { //if reverse
		// high time increases with power
		PWMduty=((int)(m->power)*(m->PWMperiod))/100;
	}

	*(m->dutyHighByte) = PWMduty; //set high duty cycle byte
        
	if (m->direction){ // if direction is high
		*(m->dir_LAT) = *(m->dir_LAT) | (1<<(m->dir_pin)); // set dir_pin bit in LAT to high without changing other bits
	} else {
		*(m->dir_LAT) = *(m->dir_LAT) & (~(1<<(m->dir_pin))); // set dir_pin bit in LAT to low without changing other bits
	}
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 0;
    mL->power = 0;
    mR->power = 0;
    setMotorPWM(mL);
    setMotorPWM(mR);
    __delay_ms(10);
}

//movement: left turn 90 degrees
void turn_left(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 1;
    for (char i = 0;i<101;i = i + 4)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(20);
    }
}

//movement: right turn 90 degrees
void turn_right(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 0;
    for (char i = 0;i<101;i = i + 4)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(20);
    }
}

//movement: straight
void forward(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 1;
    for (int i = 0;i<101;i = i+4)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(20);
    }
}

