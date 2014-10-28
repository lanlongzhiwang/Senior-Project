#include <stdio.h>
#include <stdlib.h>
#include <pwm.h>
#include <pwm12.h>
#include <p30F4012.h>
#include <p30Fxxxx.h>
#include <math.h>
#include "timer.h"
#include "adc10.h"
#include <libpic30.h>

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);                     // Turn off Watchdog Timer
_FBORPOR(PBOR_OFF & MCLR_EN);       // Set Brown-out Reset voltage and set Power-up Timer to 16 ms
_FGS(CODE_PROT_OFF);                // Set Code Protection Off

#define FCY 30000000             // 2 MIPS this is the amount of processes per second
#define FPWM 100000             // 100 kHz switching frequency
#define _0_DEGREES 0x0000       // 0 degree phase
#define _120_DEGREES 0x5555     // 120 degree phase
#define _240_DEGREES 0xAAAA     // 240 degree phase
#define _DES_FREQ (float) 120    //target frequency
#define _DELTA_PHASE (unsigned int)(_DES_FREQ * 65536 / FPWM)
#define DEADTIME (unsigned int)(0.0000002 * FCY)     //dead time between triggers 200ns


//#define DEADTIME (unsigned int)(0.00000 * FCY)

unsigned int Phase = 0, Delta_Phase;
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;
int Multiplier, Result;
const int sinetable[] = { 0,3212,6393,9512,12539,15446,18204,20787,23170,25329,

27245,28898,30273,31356,32137,32609,32767,32609,32137,31356,30273,28898,
         27245,25329,23170,20787,18204,15446,12539,9512,6393,3212,0,-
3212,-6393,
         -9512,-12539,-15446,-18204,-20787,-23170,-25329,-27245,-
28898,-30273,
         -31356,-32137,-32609,-32767,-32609,-32137,-31356,-30273,-
28898,-27245,
         -25329,-23170,-20787,-18204,-15446,-12539,-9512,-6393,-3212 
};

void __attribute__((__interrupt__, auto_psv)) _PWMInterrupt(void){

   // PWMCON2bits.UDIS = 1;

    Phase += Delta_Phase;       // Accumulate Delta_Phase in Phase variable
    Phase_Offset1 = _0_DEGREES; // Add proper value to phase offset (0 degree)
    Multiplier = sinetable[(Phase + Phase_Offset1) >> 10]; // Take sine info
    asm("MOV _Multiplier, W4"); // Load first multiplier
    asm("MOV _PTPER, W5");      // Load second multiplier
    asm("MOV #_Result, W0");    // Load W0 with the address of Result
    asm("MPY W4*W5, A");        // Perform Fractional multiplication
    asm("SAC A, [W0]");         // Store multiplication result in var Result
    PDC1 = Result + PTPER;      // Remove negative values of the duty cycle

    Phase_Offset2 = _120_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset2) >> 10];
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC2 = Result + PTPER;

    Phase_Offset3 = _240_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset3) >> 10];
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC3 = Result + PTPER;

    //PWMCON2bits.UDIS = 0;
    IFS2bits.PWMIF = 0;

}

void InitMCPWM(void){
    TRISE = 0x0100;                     // PWM pins as outputs, and FLTA as input
    //PTPER = 9;
    PTPER = (FCY/FPWM - 1) >> 1;        // Compute Period for desired frequency
    OVDCON = 0x0000;                    // Disable all PWM outputs.
    DTCON1 = DEADTIME;                  // ~200 ns of dead time @ 20 MIPS and 1:1 Prescaler
    PWMCON1 = 0x0077;                   // Enable PWM output pins and enable complementary mode
    PDC1 = PTPER;                       /* 0 Volts on Phase A. This value corresponds to
                                           50% of duty cycle, which in complementary mode 
                                           gives an average of 0 Volts */
    PDC2 = PTPER;                       // 0 Volts on Phase B.
    PDC3 = PTPER;                       // 0 Volts on Phase C.
    IFS2bits.PWMIF = 0;                 // Clear PWM Interrupt flag
    IEC2bits.PWMIE = 1;                 // Clear PWM Interrupt flag
    OVDCON = 0x3F00;                    // PWM outputs are controller by PWM module
    PTCONbits.PTMOD = 2;                // Center aligned PWM operation
    Phase = 0;                          // Reset Phase Variable
    Delta_Phase = _DELTA_PHASE;         // Initialize Phase increment for 60Hz sine wave
    PTCONbits.PTEN = 1;                 // Start PWM
    return;
}

/*void init_analog(){
    TRISB = 0x01FF;      // Port B all inputs
    ADPCFG = 0x00FF;     // Lowest 8 PORTB pins are analog inputs
    ADCON1 = 0;          // Manually clear SAMP to end sampling, start conversion
    ADCON2 = 0;          // Voltage reference from AVDD and AVSS
    ADCON3 = 0x0009;     
    ADCON1bits.ADON = 1; // Turn ADC ON
}

unsigned int read_analog()
{
    ADCHS = 0;          // Select the requested channel
    ADCON1bits.SAMP = 1;      // start sampling
    __delay32(30);            // 1us delay @ 30 MIPS
    ADCON1bits.SAMP = 0;      // start Converting
    while (!ADCON1bits.DONE);
    return ADCBUF0;
}*/

int main() {

    InitMCPWM();

    while(1){};

    return 0;
}

