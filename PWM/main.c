#include <stdio.h>
#include <stdlib.h>
#include <pwm.h>
#include <pwm12.h>
#include <p30F4012.h>
#include <p30Fxxxx.h>
#include <math.h>
#include "timer.h"
#include "adc10.h"


#define FCY 20000000 // 20 MIPS this is the amount of processes per second
#define FPWM 20000 // 20 kHz switching frequency
#define DEADTIME (unsigned int)(0.000002 * FCY) //dead time between triggers
#define _0_DEGREES 0x0000
#define _120_DEGREES 0x5555
#define _240_DEGREES 0xAAAA
#define _DES_FREQ 60 //target frequency
#define _DELTA_PHASE (unsigned int)(_DES_FREQ * 65536 / FPWM)

_FOSC(CSW_FSCM_OFF & XT_PLL4);      // Turn off clock switching and
                                    // fail-safe clock monitoring and
                                    // use the XT osc and 4x PLL as
                                    // system clock
_FWDT(WDT_OFF);                     // Turn off Watchdog Timer
_FBORPOR(PBOR_OFF & MCLR_EN);       // Set Brown-out Reset voltage and set Power-up Timer to 16 ms
_FGS(CODE_PROT_OFF); 

unsigned int Phase = 0, Delta_Phase = _DELTA_PHASE;
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;
int Multiplier1, Multiplier2, Multiplier3;
unsigned int scaler;
float des_f = _DES_FREQ;
int sinetable[] =
                    {0, 402, 804, 1206, 1608, 2009, 2411, 2811,
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

    PWMCON2bits.UDIS = 1;

    Phase += Delta_Phase; // ?Delta_Phase ???Phase ???

    Phase_Offset1 = _0_DEGREES; // ?????????????
    Multiplier1 = sinetable[(Phase + Phase_Offset1) >> 10];// ??????
    //Result1 = Multiplier1*PTPER*scaler;
    //PDC1 = Result1 + PTPER;
    asm("MOV _PTPER, W4"); // Load first multiplier (PTPER)
    asm("MOV _scaler, W5");// Load second multiplier (scaler)
    asm("MPY W4*W5, A");// Perform fractional multiplication of PTPER by scaler
    asm("SAC A, W5"); // Store scaler*PTPER in W5
    asm("MOV _Multiplier1, W4"); // Load W4 with SineSample
    asm("MPY W4*W5, A"); // Perform Fractional multiplication of SineSample by PTPER*scaler
    asm("ADD W5,A"); // Add PTPER*scaler to SineSample*PTPER*scaler
    asm("SAC A,W5");
    asm("MOV W5,PDC1");

    Phase_Offset2 = _120_DEGREES; // ?????????????
    Multiplier2 = sinetable[(Phase + Phase_Offset2) >> 10];
    asm("MOV _PTPER, W4"); // Load first multiplier (PTPER)
    asm("MOV _scaler, W5");// Load second multiplier (scaler)
    asm("MPY W4*W5, A");// Perform fractional multiplication of PTPER by scaler
    asm("SAC A, W5"); // Store scaler*PTPER in W5
    asm("MOV _Multiplier2, W4"); // Load W4 with SineSample
    asm("MPY W4*W5, A"); // Perform Fractional multiplication of SineSample by PTPER*scaler
    asm("ADD W5,A"); // Add PTPER*scaler to SineSample*PTPER*scaler
    asm("SAC A,W5");
    asm("MOV W5,PDC2");

    Phase_Offset3 = _240_DEGREES; // ?????????????
    Multiplier3 = sinetable[(Phase + Phase_Offset3) >> 10];
    asm("MOV _PTPER, W4"); // Load first multiplier (PTPER)
    asm("MOV _scaler, W5");// Load second multiplier (scaler)
    asm("MPY W4*W5, A");// Perform fractional multiplication of PTPER by scaler
    asm("SAC A, W5"); // Store scaler*PTPER in W5
    asm("MOV _Multiplier3, W4"); // Load W4 with SineSample
    asm("MPY W4*W5, A"); // Perform Fractional multiplication of SineSample by PTPER*scaler
    asm("ADD W5,A"); // Add PTPER*scaler to SineSample*PTPER*scaler
    asm("SAC A,W5");
    asm("MOV W5,PDC3");

    PWMCON2bits.UDIS = 0;
    IFS2bits.PWMIF = 0;
}

void InitMCPWM(void){

    TRISB = 0x00ff;
    TRISD = 0x0003;
    TRISE = 0x0100;

    CloseADC10();
    ADPCFG = 0xFFFF;

    IFS0bits.T1IF = 0;
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_PS_1_256 & T1_SYNC_EXT_OFF & T1_SOURCE_INT, T1VAL);
    ConfigIntTimer1(T1_INT_PRIOR_7 & T1_INT_ON);

    PTPER = ((FCY/FPWM) >> 1) - 1;
    SEVTCMP = 0;
    PTMR=0;

    PWMCON1 = 0x0077;
    PWMCON2 = 0x0011;

    DTCON1 = _DELTA_PHASE;
    OVDCON = 0x3F00;

    PDC1 = PTPER;
    PDC2 = PTPER;
    PDC3 = PTPER;

    IFS2bits.PWMIF = 0;
    IEC2bits.PWMIE = 1;

    PTCONbits.PTEN = 1;
    PTCONbits.PTCKPS=0;
    PTCONbits.PTMOD=2;

    scaler = 32767*des_f/RATED_FREQUENCY;

}

int main() {

    InitMCPWM();

    while(1);

    return 0;
}

