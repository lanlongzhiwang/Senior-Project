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
//_FOSC(CSW_FSCM_OFF & FRC_PLL4);
_FWDT(WDT_OFF);                     // Turn off Watchdog Timer
_FBORPOR(PBOR_OFF & MCLR_EN);       // Set Brown-out Reset voltage and set Power-up Timer to 16 ms
_FGS(CODE_PROT_OFF);                // Set Code Protection Off

//#define FCY 29840000             // 30 MIPS this is the amount of processes per second
#define FCY 29400000
#define FPWM 65000             // 100 kHz switching frequency
#define _0_DEGREES 0x0000       // 0 degree phase
#define _120_DEGREES 0x5555     // 120 degree phase
#define _240_DEGREES 0xAAAA     // 240 degree phase
#define _DES_FREQ (float) 10    //target frequency
#define _DELTA_PHASE (unsigned int)(_DES_FREQ * 65536 / FPWM)
#define DEADTIME (unsigned int)(0.0000002 * FCY)     //dead time between triggers 200ns


//#define DEADTIME (unsigned int)(0.00000 * FCY)

unsigned int Phase = 0;
unsigned int Delta_Phase, check;
double Delta_Phase_real;
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;
unsigned int Multiplier, Result;
unsigned char RAMBuffer[256];	//RAM area which will work as EEPROM for Master I2C device
unsigned char *RAMPtr;			//Pointer to RAM memory locations
int AddrFlag = 0;	//Initlize AddFlag
int DataFlag = 0;	//Initlize DataFlag
const int sinetable[] = {0, 3212, 6393, 9512, 12539, 15446, 18204, 20787, 23170,
25329, 27245, 28898, 30273, 31356, 32137, 32609, 32767, 32609, 32137, 31356, 30273,
28898, 27245, 25329, 23170, 20787, 18204, 15446, 12539, 9512, 6393, 3212, 0, -3212,
-6393, -9512, -12539, -15446, -18204, -20787, -23170, -25329, -27245, -28898, -30273,
-31356, -32137, -32609, -32767, -32609, -32137, -31356, -30273, -28898, -27245, -25329,
-23170, -20787, -18204, -15446, -12539, -9512, -6393, -3212
};

void __attribute__((interrupt, no_auto_psv)) _PWMInterrupt(void){

    Phase += Delta_Phase;       // Accumulate Delta_Phase in Phase variable
    //Phase2 += Delta_Phase_real;
    Phase_Offset1 = _0_DEGREES; // Add proper value to phase offset (0 degree)
    Multiplier = sinetable[(Phase + Phase_Offset1) >> 10]; // Take sine info

    //Multiplier = (Multiplier1 + Multiplier2) >> 1;

    asm("MOV _Multiplier, W4"); // Load first multiplier
    asm("MOV _PTPER, W5");      // Load second multiplier
    asm("MOV #_Result, W0");    // Load W0 with the address of Result
    asm("MPY W4*W5, A");        // Perform Fractional multiplication
    asm("SAC A, [W0]");         // Store multiplication result in var Result

    PDC1 = Result + PTPER + RAMBuffer[9];

    Phase_Offset2 = _120_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset2) >> 10]; // Take sine info
    //Multiplier2 = sinetable[(Phase2 + Phase_Offset2) >> 9]; // Take sine info
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC2 = Result + PTPER + RAMBuffer[9];

    Phase_Offset3 = _240_DEGREES;
    Multiplier = sinetable[(Phase + Phase_Offset3) >> 10]; // Take sine info
    //Multiplier2 = sinetable[(Phase2 + Phase_Offset3) >> 9]; // Take sine info
    asm("MOV _Multiplier, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC3 = Result + PTPER + RAMBuffer[9];

    IFS2bits.PWMIF = 0;

}
void __attribute__((interrupt,no_auto_psv)) _SI2CInterrupt(void){
	unsigned char Temp;	//used for dummy read

	if((I2CSTATbits.R_W == 0)&&(I2CSTATbits.D_A == 0))	//Address matched
		{
			Temp = I2CRCV;		//dummy read
			AddrFlag = 1;	//next byte will be address
		}
	else if((I2CSTATbits.R_W == 0)&&(I2CSTATbits.D_A == 1))	//check for data
		{
			if(AddrFlag)
			{
				AddrFlag = 0;
				DataFlag = 1;	//next byte is data
				RAMPtr = RAMPtr + I2CRCV;
			}
			else if(DataFlag)
			{
				*RAMPtr = (unsigned char)I2CRCV;// store data into RAM
                                AddrFlag = 0;//end of tx
				DataFlag = 0;
				RAMPtr = &RAMBuffer[0];	//reset the RAM pointer
			}
		}
	else if((I2CSTATbits.R_W == 1)&&(I2CSTATbits.D_A == 0))
	{
		Temp = I2CRCV;
		I2CTRN = *RAMPtr;	//Read data from RAM & send data to I2C master device
		I2CCONbits.SCLREL = 1;	//Release SCL1 line
		while(I2CSTATbits.TBF);//Wait till all
		RAMPtr = &RAMBuffer[0];	//reset the RAM pointer
	}
	_SI2CIF = 0;	//clear I2C1 Slave interrupt flag
}

void InitMCPWM(void){
    TRISE = 0x0100;                     // PWM pins as outputs, and FLTA as input
    //PTPER = 9;
    PTPER = (FCY/FPWM -1) >> 1;        // Compute Period for desired frequency
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
    PTCONbits.PTEN = 1;                 // Start PWM
    return;
}
void i2c_init(void){
        LATD = 0;
        TRISD = 0b11111110;
	I2CCON = 0x8000;	//Enable I2C1 module
	I2CADD = 0x77;			// 7-bit I2C slave address must be initialised here.

	IFS1=0;
	RAMPtr = &RAMBuffer[0];	//set the RAM pointer and points to beginning of RAMBuffe
	_SI2CIE = 1;
}

int main() {

    RAMBuffer[7] = 10;
    RAMBuffer[9] = 0;
    i2c_init();
    InitMCPWM();

    _LATD0 = 1;
    while(1){

        Delta_Phase = (RAMBuffer[7] * 65536 / FPWM);

    };

    return 0;
}

