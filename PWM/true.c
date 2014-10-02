#include "p33FJ128MC706.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#define FPWM 20000                   //20kHz
#define FREQ_DES 60                  //Frequence désirée
#define FCY 40000000                 // Fcy
#define SIN_FREQ 60                  // 60 Hz 
#define Angle_120_degree 0x5555
#define Angle_240_degree 0xAAAA
#define DELTA_PHASE (unsigned int)(FREQ_DES*65536/FPWM)

//void __attribute__((__interrupt__))_PWMInterrupt(void); 
//void MCPWM(void);

_FWDT();
_FOSCSEL(FNOSC_FRCPLL);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FPOR(PWMPIN_ON & HPOL_ON & LPOL_ON);
//_FBORPOR(PBOR_OFF & MCLR_EN);

unsigned int PWM_PERIOD;
unsigned int Phase=0;

int Multiplier, Result;



int SineTable[] = {0,3212,6393,9512,12539,15446,18204,20787,23170,25329,
					27245,28898,30273,31356,32137,32609,32767,32609,32137,31356,30273,28898,
					27245,25329,23170,20787,18204,15446,12539,9512,6393,3212,0,-3212,-6393,
					-9512,-12539,-15446,-18204,-20787,-23170,-25329,-27245,-28898,-30273,
					-31356,-32137,-32609,-32767,-32609,-32137,-31356,-30273,-28898,-27245,
					-25329,-23170,-20787,-18204,-15446,-12539,-9512,-6393,-3212};

void MCPWM(void)
{

   P1TCONbits.PTEN = 0;
   P1TCONbits.PTCKPS=0;
   P1TCONbits.PTMOD=2;
   P1TMR=0;

   PWM_PERIOD=((FCY/FPWM)>>1)-1;
   P1TPER = PWM_PERIOD;
   P1SECMPbits.SEVTCMP = 0;
   
   PWM1CON1=0x0077;
   PWM1CON2bits.IUE = 1;
   PWM1CON2bits.OSYNC = 1;
   PWM1CON2=0x0010;

   P1DTCON1bits.DTA=10;
   P1DTCON1bits.DTB=20;
   
   P1DTCON2=0x0000;
   
   P1OVDCON=0x3F00;
   PWM1CON2bits.UDIS = 1;	
   P1DC1 = PWM_PERIOD;
   P1DC2 = PWM_PERIOD/2;
   P1DC3 = PWM_PERIOD-P1DC1/2;
	PWM1CON2bits.UDIS = 0;
   Phase=0;  

   _PWMIE=1;
   //_PWMIP=3;
   _PWMIF=0;
   P1TCONbits.PTEN = 1;

  

   
}

int main()
{
// Configure the oscillator to operate the device at 40 MHz
// The Fast RC (FRC) internal oscillator runs at a nominal frequency of 7.37 MHz
// FOSC = Fin * M/(N1 * N2), FCY = FOSC/2
// FOSC = 7.37 * (43)/(2 * 2) = 80 MHz for FOSC, FCY = 40 MHz
// In order to configure the device to operate at 40 MHz, configure the PLL prescaler,
// PLL postscaler, and PLL divisor
	PLLFBD = 41; // M = PLLFBD + 2
	CLKDIVbits.PLLPOST = 0; // N1 = 2
	CLKDIVbits.PLLPRE = 0; // N2 = 2
	__builtin_write_OSCCONH(0x01); // New oscillator FRC w/ PLL
	__builtin_write_OSCCONL(0x01); // Enable clock switch as per dsPIC oscillator start-up

	while(OSCCONbits.COSC!= 0b001); // Wait for the new Oscillator to become FRC w/ PLL
	while(OSCCONbits.LOCK!= 1); // Wait for PLL to lock

 //ADPCFG = 0xFFFF;
 TRISB=0x0000;
 TRISE=0x0100;
 
 MCPWM();
// Update PDC1 register
	Phase += DELTA_PHASE;// Accumulate Delta_Phase in Phase variable
	Multiplier = SineTable[Phase >> 10];// Take sine info
	asm("MOV _Multiplier, W4"); // Load first multiplier
	asm("MOV _PTPER, W5");// Load second multiplier
	asm("MOV #_Result, W0");// Load W0 with the address of Result
	asm("MPY W4*W5, A");// Perform Fractional multiplication
	asm("SAC A, [W0]");// Store multiplication result in var Result
	P1DC1 = Result + PTPER;// Remove negative values of the duty cycle

	// Update PDC2 register
    
	Multiplier = SineTable[(Phase + Angle_120_degree) >> 10];// Take sine info
	asm("MOV _Multiplier, W4"); // Load first multiplier
	asm("MOV _PTPER, W5");// Load second multiplier
	asm("MOV #_Result, W0");// Load W0 with the address of Result
	asm("MPY W4*W5, A");// Perform Fractional multiplication
	asm("SAC A, [W0]");// Store multiplication result in var Result
	P1DC2 = Result + PTPER;// Remove negative values of the duty cycle

	// Update PDC3 register
   
	Multiplier = SineTable[(Phase + Angle_240_degree) >> 10];// Take sine info
	asm("MOV _Multiplier, W4"); // Load first multiplier
	asm("MOV _PTPER, W5");// Load second multiplier
	asm("MOV #_Result, W0");// Load W0 with the address of Result
	asm("MPY W4*W5, A");// Perform Fractional multiplication
	asm("SAC A, [W0]");// Store multiplication result in var Result
	P1DC3 = Result + PTPER;// Remove negative values of the duty cycle

   

 while(1)
 {
   }
} 


void __attribute__((__interrupt__))_PWMInterrupt(void)
{
    
    _PWMIF = 0; 

}


	