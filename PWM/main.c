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


#define FCY 20000000            // 20 MIPS this is the amount of processes per second
#define FPWM 100000              // 100 kHz switching frequency
#define _0_DEGREES 0x0000       // 0 degree phase
#define _120_DEGREES 0x5555     // 120 degree phase
#define _240_DEGREES 0xAAAA     // 240 degree phase
#define _DES_FREQ (float) 120    //target frequency
#define _DELTA_PHASE (unsigned int)(_DES_FREQ * 65536 / FPWM)
//#define DEADTIME (unsigned int)(0.000002 * FCY)     //dead time between triggers
#define DEADTIME (unsigned int)(0.00000 * FCY)

unsigned int Phase = 0, Delta_Phase;
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;
int Multiplier, Result;
int sinetable[] = {0, 402, 804, 1206, 1608, 2009, 2411, 2811,
 3212, 3612, 4011, 4410, 4808, 5205, 5602, 5998,
 6393, 6787, 7180, 7571, 7962, 8351, 8740, 9127,
 9512, 9896, 10279, 10660, 11039, 11417, 11793, 12167,
 12540, 12910, 13279, 13646, 14010, 14373, 14733, 15091,
 15447, 15800, 16151, 16500, 16846, 17190, 17531, 17869,
 18205, 18538, 18868, 19195, 19520, 19841, 20160, 20475,
 20788, 21097, 21403, 21706, 22006, 22302, 22595, 22884,
 23170, 23453, 23732, 24008, 24279, 24548, 24812, 25073,
 25330, 25583, 25833, 26078, 26320, 26557, 26791, 27020,
 27246, 27467, 27684, 27897, 28106, 28311, 28511, 28707,
 28899, 29086, 29269, 29448, 29622, 29792, 29957, 30118,
 30274, 30425, 30572, 30715, 30853, 30986, 31114, 31238,
 31357, 31471, 31581, 31686, 31786, 31881, 31972, 32058,
 32138, 32214, 32286, 32352, 32413, 32470, 32522, 32568,
 32610, 32647, 32679, 32706, 32729, 32746, 32758, 32766,
 32768, 32766, 32758, 32746, 32729, 32706, 32679, 32647,
 32610, 32568, 32522, 32470, 32413, 32352, 32286, 32214,
 32138, 32058, 31972, 31881, 31786, 31686, 31581, 31471,
 31357, 31238, 31114, 30986, 30853, 30715, 30572, 30425,
 30274, 30118, 29957, 29792, 29622, 29448, 29269, 29086,
 28899, 28707, 28511, 28311, 28106, 27897, 27684, 27467,
 27246, 27020, 26791, 26557, 26320, 26078, 25833, 25583,
 25330, 25073, 24812, 24548, 24279, 24008, 23732, 23453,
 23170, 22884, 22595, 22302, 22006, 21706, 21403, 21097,
 20788, 20475, 20160, 19841, 19520, 19195, 18868, 18538,
 18205, 17869, 17531, 17190, 16846, 16500, 16151, 15800,
 15447, 15091, 14733, 14373, 14010, 13646, 13279, 12910,
 12540, 12167, 11793, 11417, 11039, 10660, 10279, 9896,
 9512, 9127, 8740, 8351, 7962, 7571, 7180, 6787,
 6393, 5998, 5602, 5205, 4808, 4410, 4011, 3612,
 3212, 2811, 2411, 2009, 1608, 1206, 804, 402,
 0,-402,-804,-1206,-1608,-2009,-2411,-2811,
-3212,-3612,-4011,-4410,-4808,-5205,-5602,-5998,
-6393,-6787,-7180,-7571,-7962,-8351,-8740,-9127,
-9512,-9896,-10279,-10660,-11039,-11417,-11793,-12167,
-12540,-12910,-13279,-13646,-14010,-14373,-14733,-15091,
-15447,-15800,-16151,-16500,-16846,-17190,-17531,-17869,
-18205,-18538,-18868,-19195,-19520,-19841,-20160,-20475,
-20788,-21097,-21403,-21706,-22006,-22302,-22595,-22884,
-23170,-23453,-23732,-24008,-24279,-24548,-24812,-25073,
-25330,-25583,-25833,-26078,-26320,-26557,-26791,-27020,
-27246,-27467,-27684,-27897,-28106,-28311,-28511,-28707,
-28899,-29086,-29269,-29448,-29622,-29792,-29957,-30118,
-30274,-30425,-30572,-30715,-30853,-30986,-31114,-31238,
-31357,-31471,-31581,-31686,-31786,-31881,-31972,-32058,
-32138,-32214,-32286,-32352,-32413,-32470,-32522,-32568,
-32610,-32647,-32679,-32706,-32729,-32746,-32758,-32766,
-32768,-32766,-32758,-32746,-32729,-32706,-32679,-32647,
-32610,-32568,-32522,-32470,-32413,-32352,-32286,-32214,
-32138,-32058,-31972,-31881,-31786,-31686,-31581,-31471,
-31357,-31238,-31114,-30986,-30853,-30715,-30572,-30425,
-30274,-30118,-29957,-29792,-29622,-29448,-29269,-29086,
-28899,-28707,-28511,-28311,-28106,-27897,-27684,-27467,
-27246,-27020,-26791,-26557,-26320,-26078,-25833,-25583,
-25330,-25073,-24812,-24548,-24279,-24008,-23732,-23453,
-23170,-22884,-22595,-22302,-22006,-21706,-21403,-21097,
-20788,-20475,-20160,-19841,-19520,-19195,-18868,-18538,
-18205,-17869,-17531,-17190,-16846,-16500,-16151,-15800,
-15447,-15091,-14733,-14373,-14010,-13646,-13279,-12910,
-12540,-12167,-11793,-11417,-11039,-10660,-10279,-9896,
-9512,-9127,-8740,-8351,-7962,-7571,-7180,-6787,
-6393,-5998,-5602,-5205,-4808,-4410,-4011,-3612,
-3212,-2811,-2411,-2009,-1608,-1206,-804,-402
};

void __attribute__((__interrupt__, auto_psv)) _PWMInterrupt(void){

    Phase += Delta_Phase;       // Accumulate Delta_Phase in Phase variable
    Phase_Offset1 = _0_DEGREES; // Add proper value to phase offset (0 degree)
    Multiplier = sinetable[(Phase + Phase_Offset1) >> 5]; // Take sine info
    asm("MOV _Multiplier, W4"); // Load first multiplier
    asm("MOV _PTPER, W5");      // Load second multiplier
    asm("MOV #_Result, W0");    // Load W0 with the address of Result
    asm("MPY W4*W5, A");        // Perform Fractional multiplication
    asm("SAC A, [W0]");         // Store multiplication result in var Result
    PDC1 = Result + PTPER;      // Remove negative values of the duty cycle

    Phase_Offset2 = _120_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset2) >> 5];
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC2 = Result + PTPER;

    Phase_Offset3 = _240_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset3) >> 5];
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC3 = Result + PTPER;

}

void InitMCPWM(void){
    TRISE = 0x0100;                     // PWM pins as outputs, and FLTA as input
    PTPER = (FCY/FPWM - 1) >> 1;        // Compute Period for desired frequency
    OVDCON = 0x0000;                    // Disable all PWM outputs.
    DTCON1 = DEADTIME;                  // ~2 us of dead time @ 20 MIPS and 1:1 Prescaler
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

