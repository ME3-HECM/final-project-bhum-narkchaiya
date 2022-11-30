#include <xc.h>
#include "interrupts.h"
#include "comparator.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
    PIE2bits.C1IE=1; //enable interrupt source Comparator 1
    IPR2bits.C1IP=1; //set interrupt as high priority
    PIR2bits.C1IF=0; //interrupt initially set as off

    INTCONbits.IPEN=1;
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1; //turn on interrupts globally (when this is off, all interrupts are deactivated)
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if(PIR2bits.C1IF){					//check the interrupt source
        LATHbits.LATH3 = !LATHbits.LATH3; //toggle H3 LED
        PIR2bits.C1IF=0; 				 //clear the interrupt flag!
	}
}

