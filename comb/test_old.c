#include <stdio.h>
#include <stdlib.h>
#include <pwm.h>
#include <pwm12.h>
#include <p30F5015.h>
#include <p30Fxxxx.h>
#include <math.h>
#include "timer.h"
#include "adc10.h"
#include <libpic30.h>

_FOSC(CSW_FSCM_OFF & FRC_PLL16);                    // Fail-Safe Clock Monito turn off
                                                    // 7.37 MHz internal RC oscillator,
                                                    // 16x PLL enabled
                                                    // 7.37M*16/4 = 29.84 Mhz
//_FBORPOR(MCLR_DIS);                                 // Disable the MCLR
 _FWDT(WDT_OFF);                                    // Disable the watch dog

#define FCY 29840000                                // 29.84 MIPS this is the amount of
                                                    // processes per second Tcy = 33 ns
#define FPWM 65000                                  // 65 kHz switching frequency
#define _0_DEGREES 0x0000                           // 0 degree phase
#define _120_DEGREES 0x5555                         // 120 degree phase
#define _240_DEGREES 0xAAAA                         // 240 degree phase
#define _DES_FREQ (float) 10                        // Target frequency
#define DEADTIME (unsigned int)(0.000000200 * FCY)    // Dead time between triggers 400ns

unsigned int Phase = 0;                                   // Initialize Phase variable
unsigned int Delta_Phase;                                 // Declare Delta Phase
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3; // Declare 3-phase PWM each offset
unsigned int Multiplier, Result;                          // Declare some variable use in
                                                          // asm() function

//sinetable with 64 entry (special)
/*const int sinetable[] ={
    16383,17989,19580,21140,22653,24107,25486,26777,27969,29049,30006,30833,31520,
    32062,32453,32689,32767,32689,32453,32062,31520,30833,30006,29049,27969,26777,
    25486,24107,22653,21140,19580,17989,16384,14778,13187,11627,10114,8660,7281,5990,
    4798,3718,2761,1934,1247,705,314,78, 0,78,314,705,1247,1934,2761,3718,4798,5990,
    7281,8660,10114,11627,13187,14778

};*/

//sinetable with 64 entry (regular)
const int sinetable[] = {0, 3212, 6393, 9512, 12539, 15446, 18204, 20787, 23170,
                         25329, 27245, 28898, 30273, 31356, 32137, 32609, 32767,
                         32609, 32137, 31356, 30273, 28898, 27245, 25329, 23170,
                         20787, 18204, 15446, 12539, 9512, 6393, 3212, 0, -3212,
                         -6393, -9512, -12539, -15446, -18204, -20787, -23170,
                         -25329, -27245, -28898, -30273, -31356, -32137, -32609,
                         -32767, -32609, -32137, -31356, -30273, -28898, -27245,
                         -25329, -23170, -20787, -18204, -15446, -12539, -9512,
                         -6393, -3212};

//Initializev PWM mode
void PWM_init(void){

    TRISE = 0x0100;             // PWM pins as outputs, and FLTA as input
    PTPER = (FCY/FPWM -1) >> 1; // Compute Period for desired frequency
    OVDCON = 0x0000;            // Disable all PWM outputs.
    DTCON1 = DEADTIME;          // 400 ns of dead time @ 20 MIPS and 1:1 Prescaler
    PWMCON1 = 0x0077;           // Enable PWM output pins and enable complementary mode
    PDC1 = PTPER;               // Initialize 50% duty cycle of first PWM
    PDC2 = PTPER;               // Initialize 50% duty cycle of second PWM
    PDC3 = PTPER;               // Initialize 50% duty cycle of third PWM
    IFS2bits.PWMIF = 0;         // Clear PWM Interrupt flag
    IEC2bits.PWMIE = 1;         // Clear PWM Interrupt flag
    OVDCON = 0x3F00;            // PWM outputs are controller by PWM module
    PTCONbits.PTMOD = 2;        // Center aligned PWM operation
    PTCONbits.PTEN = 1;         // Start PWM
    return;
}

//PWM main loop
void __attribute__((interrupt, no_auto_psv)) _PWMInterrupt(void){

    PWMCON2bits.UDIS = 1;             // Updates from duty cycle and period buffer
                                      // registers are disabled

    Phase += Delta_Phase;             // Accumulate Delta_Phase in Phase variable
    Phase_Offset1 = _0_DEGREES;       // Add proper value to phase offset (0 degree)
    Multiplier = sinetable[(Phase + Phase_Offset1) >> 10];  // Take sinetable info

    asm("MOV _Multiplier, W4");       // Load first multiplier
    asm("MOV _PTPER, W5");            // Load second multiplier
    asm("MOV #_Result, W0");          // Load W0 with the address of Result
    asm("MPY W4*W5, A");              // Perform Fractional multiplication
    asm("SAC A, [W0]");               // Store multiplication result in var Result

    PDC1 = Result + PTPER;            // The duty cycle of first PWM

    Phase_Offset2 = _120_DEGREES;     // Add proper value to phase offset (120 degree)
    Multiplier = sinetable[(Phase + Phase_Offset2) >> 10];  // Take sinetable info

    asm("MOV _Multiplier, W4");       // Load first multiplier
    asm("MOV _PTPER, W5");            // Load second multiplier
    asm("MOV #_Result, W0");          // Load W0 with the address of Result
    asm("MPY W4*W5, A");              // Perform Fractional multiplication
    asm("SAC A, [W0]");               // Store multiplication result in var Result

    PDC2 = Result + PTPER;            // The duty cycle of second PWM

    Phase_Offset3 = _240_DEGREES;     // Add proper value to phase offset (240 degree)
    Multiplier = sinetable[(Phase + Phase_Offset3) >> 10];  // Take sinetable inf

    asm("MOV _Multiplier, W4");       // Load first multiplier
    asm("MOV _PTPER, W5");            // Load second multiplier
    asm("MOV #_Result, W0");          // Load W0 with the address of Result
    asm("MPY W4*W5, A");              // Perform Fractional multiplication
    asm("SAC A, [W0]");               // Store multiplication result in var Result

    PDC3 = Result + PTPER;            // The duty cycle of third PWM

    PWMCON2bits.UDIS = 0;             // Updates from duty cycle and period buffer registers
                                      // are enabled
    IFS2bits.PWMIF = 0;               // Clear PWM Interrupt Request Flags

}
int main() {

    PWM_init();                                             // Initialize PWM mode

    //RAMBuffer[frequency] = 17;                              // Initialize frequency to 17 Hz

    while(1){

        // Calculate delta phase base on frequency
        Delta_Phase = (27 * 65536 / FPWM);

    };

    return 0;
}

