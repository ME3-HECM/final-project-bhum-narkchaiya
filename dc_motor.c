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
    T2PR=PWMperiod;// * 2000000 - 1; //Period reg 10kHz base period, for 10kHz, the time slot is 1*10^-4, prescaler should be smaller than this
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
    for (int i = 5;i>0;i = i - 1)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(10);
    }
    
}

//function to make the robot go straight
void forward(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 1;
    for (int i=0;i<31;i=i+2)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(20);
    }
}

void reverse_fromcard (struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mL->direction = 0;
    for (int i = 0;i < 41;i = i + 2)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(500);
}

void reverse_onesquare (struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 0;
    for (int i=0;i<51;i=i + 2)
    {
        mL->power = i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(1100);
}

void right_90(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 0;
    for (int i = 0;i<50;i = i + 2)
    {
        mL->power = i;
        mR->power = 50 + i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(165);
}

void left_90(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 1;
    for (int i = 0;i<50;i = i + 2)
    {
        mL->power = 50 + i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(165);
}

void spin_180(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 0;
    for (int i = 0;i<50;i = i + 2)
    {
        mL->power = i;
        mR->power = 50 + i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(290);
}

void right_135(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 0;
    for (int i = 0;i<50;i = i + 2)
    {
        mL->power = i;
        mR->power = 50 + i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(230);
}

void left_135(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 1;
    for (int i = 0;i<50;i = i + 2)
    {
        mL->power = 50 + i;
        mR->power = i;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(10);
    }
    __delay_ms(230);
}

void motor_action(unsigned int color, struct DC_motor *l, struct DC_motor *r)
{//1,2,3 fixed
    switch (color){
        case 1: //red: right 90
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            right_90(l,r);
            stop(l,r);
            break;
        case 2: //green: left 90
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500); 
            left_90(l,r);
            stop(l,r);
            break;
        case 3: //blue: turn 180
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            spin_180(l,r);
            stop(l,r);
            break;
        case 4: //yellow: rev, right 90
            reverse_onesquare(l,r);
            stop(l,r);
            __delay_ms(500);
            right_90(l,r);
            stop(l,r);
            break;
        case 5: //pink: rev, left 90
            reverse_onesquare(l,r);
            stop(l,r);
            __delay_ms(500);
            left_90(l,r);
            stop(l,r);
            break;
        case 6: //orange: right 135
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            right_135(l,r);
            stop(l,r);
            break;
        case 7: //light blue: left 135
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            left_135(l,r);
            stop(l,r);
            break;
        case 0:
            forward(l,r);
            stop(l,r);
            break;
        default:
            break;
    }
}

void motor_action_return(unsigned int color, struct DC_motor *l, struct DC_motor *r)
{//4 & 5 not fixed
    switch (color){ //opposite
        case 1: //red: right 90
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            left_90(l,r);
            stop(l,r);
            break;
        case 2: //green: left 90
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            right_90(l,r);
            stop(l,r);
            break;
        case 3: //blue: turn 180
            reverse_fromcard(l,r);
            stop(l,r);
            __delay_ms(500);
            spin_180(l,r);
            stop(l,r);
            break;
        case 4: //yellow: rev, right 90
            stop(l,r);
            right_90(l,r);
            forward(l,r);
            stop(l,r);
            break;
        case 5: //pink: rev, left 90
            stop(l,r);
            left_90(l,r);
            forward(l,r);
            stop(l,r);
            break;
        case 6: //orange: right 135
            stop(l,r);
            left_135(l,r);
            stop(l,r);
            break;
        case 7: //light blue: left 135
            stop(l,r);
            right_135(l,r);
            stop(l,r);
            break;
        default:
            break;
    }
}