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

#define FCY 29480000             // 30 MIPS this is the amount of processes per second
#define FPWM 100000             // 100 kHz switching frequency
#define _0_DEGREES 0x0000       // 0 degree phase
#define _120_DEGREES 0x5555     // 120 degree phase
#define _240_DEGREES 0xAAAA     // 240 degree phase
#define _DES_FREQ (float) 10    //target frequency
#define _DELTA_PHASE (unsigned int)(_DES_FREQ * 65536 / FPWM)
#define DEADTIME (unsigned int)(0.0000002 * FCY)     //dead time between triggers 200ns


//#define DEADTIME (unsigned int)(0.00000 * FCY)

unsigned int Phase1 = 0;
unsigned int Phase2 = 0;
unsigned int Delta_Phase1, Delta_Phase2;
double Delta_Phase_real, right1, right2;
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;
unsigned int Multiplier, Multiplier1, Multiplier2, Result_a, Result_b, Result;
unsigned char RAMBuffer[256];	//RAM area which will work as EEPROM for Master I2C device
unsigned char *RAMPtr;			//Pointer to RAM memory locations
int AddrFlag = 0;	//Initlize AddFlag
int DataFlag = 0;	//Initlize DataFlag
const int sinetable[] = {0, 1608, 3212, 4808, 6393, 7962, 9512, 11039, 12539, 14010,
15446, 16846, 18204, 19519, 20787, 22005, 23170, 24279, 25329, 26319, 27245, 28105,
28898, 29621, 30273, 30852, 31356, 31785, 32137, 32412, 32609, 32728, 32767, 32728,
32609, 32412, 32137, 31785, 31356, 30852, 30273, 29621, 28898, 28105, 27245, 26319,
25329, 24279, 23170, 22005, 20787, 19519, 18204, 16846, 15446, 14010, 12539, 11039,
9512, 7962, 6393, 4808, 3212, 1608, 0, -1608, -3212, -4808, -6393, -7962, -9512, -11039,
-12539, -14010, -15446, -16846, -18204, -19519, -20787, -22005, -23170, -24279, -25329,
-26319, -27245, -28105, -28898, -29621, -30273, -30852, -31356, -31785, -32137, -32412,
-32609, -32728, -32767, -32728, -32609, -32412, -32137, -31785, -31356, -30852, -30273,
-29621, -28898, -28105, -27245, -26319, -25329, -24279, -23170, -22005, -20787, -19519,
-18204, -16846, -15446, -14010, -12539, -11039, -9512, -7962, -6393, -4808, -3212, -1608
};

void __attribute__((__interrupt__, auto_psv)) _PWMInterrupt(void){

    Delta_Phase1 = Delta_Phase_real;
    //Delta_Phase2 = Delta_Phase1 + 1;
    Phase1 += Delta_Phase1;       // Accumulate Delta_Phase in Phase variable
    //Phase2 += Delta_Phase2;
    //Phase2 += Delta_Phase_real;
    Phase_Offset1 = _0_DEGREES; // Add proper value to phase offset (0 degree)
    Multiplier1 = sinetable[(Phase1 + Phase_Offset1) >> 9]; // Take sine info
   //Multiplier2 = sinetable[(Phase2 + Phase_Offset1) >> 9]; // Take sine info

    //Multiplier = (Multiplier1 + Multiplier2) >> 1;

    asm("MOV _Multiplier1, W4"); // Load first multiplier
    asm("MOV _PTPER, W5");      // Load second multiplier
    asm("MOV #_Result, W0");    // Load W0 with the address of Result
    asm("MPY W4*W5, A");        // Perform Fractional multiplication
    asm("SAC A, [W0]");         // Store multiplication result in var Result
    
    /*asm("MOV _Multiplier2, W4"); // Load first multiplier
    asm("MOV _PTPER, W5");      // Load second multiplier
    asm("MOV #_Result, W0");    // Load W0 with the address of Result
    asm("MPY W4*W5, A");        // Perform Fractional multiplication
    asm("SAC A, [W0]");         // Store multiplication result in var Result*/
    
    //PDC1 = Result_b + PTPER;      // Remove negative values of the duty cycle
    //PDC1 = ( (Result_a + PTPER) + (Result + PTPER) )/2;      // Remove negative values of the duty cycle
    PDC1 = Result + PTPER + RAMBuffer[9];
    //PDC1 = Result_a + PTPER;
    //PDC2 = Result + PTPER + 50;

    Phase_Offset2 = _120_DEGREES;
    Multiplier1 = sinetable[(Phase1 + Phase_Offset2) >> 9]; // Take sine info
    //Multiplier2 = sinetable[(Phase2 + Phase_Offset2) >> 9]; // Take sine info
    asm("MOV _Multiplier1, W4");
    asm("MOV _PTPER, W5");
    asm("MOV #_Result, W0");
    asm("MPY W4*W5, A");
    asm("SAC A, [W0]");
    PDC2 = Result + PTPER + RAMBuffer[9];

    Phase_Offset3 = _240_DEGREES;
    Multiplier1 = sinetable[(Phase1 + Phase_Offset3) >> 9]; // Take sine info
    //Multiplier2 = sinetable[(Phase2 + Phase_Offset3) >> 9]; // Take sine info
    asm("MOV _Multiplier1, W4");
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

        Delta_Phase_real = (RAMBuffer[7] * 65535 / FPWM);
        //Delta_Phase1 = Delta_Phase_real;

    };

    return 0;
}

