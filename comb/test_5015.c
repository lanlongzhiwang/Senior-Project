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
                                                    // 7.37 MHz internal RC oscillator, 16x PLL enabled
                                                    // 7.37M*16/4 = 29.84 Mhz

#define FCY 29840000                                // 29.84 MIPS this is the amount of processes per second
                                                    // Tcy = 33 ns
#define FPWM 65000                                  // 65 kHz switching frequency
#define _0_DEGREES 0x0000                           // 0 degree phase
#define _120_DEGREES 0x5555                         // 120 degree phase
#define _240_DEGREES 0xAAAA                         // 240 degree phase
#define _DES_FREQ (float) 10                        //target frequency
#define DEADTIME (unsigned int)(0.0000004 * FCY)    //dead time between triggers 400ns

#define frequency 0                                 // Define frequency register is 0
#define PWM1_voltage_h 1                            // Define PWM1 voltage higher 8-bits register is 1
#define PWM1_voltage_l 2                            // Define PWM1 voltage lowr 8-bits register is 2
#define PWM1_current_h 3                            // Define PWM1 current higher 8-bits register is 3
#define PWM1_current_l 4                            // Define PWM1 current lowr 8-bits register is 4
#define PWM2_voltage_h 5                            // Define PWM2 voltage higher 8-bits register is 5
#define PWM2_voltage_l 6                            // Define PWM2 voltage lowr 8-bits register is 6
#define PWM2_current_h 7                            // Define PWM2 current higher 8-bits register is 7
#define PWM2_current_l 8                            // Define PWM2 current lowr 8-bits register is 8
#define PWM3_voltage_h 9                            // Define PWM3 voltage higher 8-bits register is 9
#define PWM3_voltage_l 10                           // Define PWM3 voltage lowr 8-bits register is 10
#define PWM3_current_h 11                           // Define PWM3 current higher 8-bits register is 11
#define PWM3_current_l 12                           // Define PWM3 current lowr 8-bits register is 12
#define DC_voltage_h 13                             // Define PWM2 voltage higher 8-bits register is 13
#define DC_voltage_l 14                             // Define PWM2 voltage lowr 8-bits register is 14

unsigned int Phase = 0;                                         // Initialize Phase variable
unsigned int Delta_Phase;                                       // Declare Delta Phase
unsigned int Phase_Offset1, Phase_Offset2, Phase_Offset3;       // Declare 3-phase PWM each offset
unsigned int Multiplier, Result;                                // Declare some variable use in asm() function
unsigned char RAMBuffer[256];                                   // RAM Buffer
unsigned char *RAMPtr;                                          // Pointer to RAM memory locations
int AddrFlag = 0;                                               // Initialize AddressFlag
int DataFlag = 0;                                               // Initialize DataFlag

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
   
    

    TRISE = 0x0100;                     // PWM pins as outputs, and FLTA as input
    PTPER = (FCY/FPWM -1) >> 1;         // Compute Period for desired frequency
    OVDCON = 0x0000;                    // Disable all PWM outputs.
    DTCON1 = DEADTIME;                  // 400 ns of dead time @ 20 MIPS and 1:1 Prescaler
    PWMCON1 = 0x0077;                   // Enable PWM output pins and enable complementary mode
    PDC1 = PTPER;                       // Initialize 50% duty cycle of first PWM
    PDC2 = PTPER;                       // Initialize 50% duty cycle of second PWM
    PDC3 = PTPER;                       // Initialize 50% duty cycle of third PWM
    IFS2bits.PWMIF = 0;                 // Clear PWM Interrupt flag
    IEC2bits.PWMIE = 1;                 // Clear PWM Interrupt flag
    OVDCON = 0x3F00;                    // PWM outputs are controller by PWM module
    PTCONbits.PTMOD = 2;                // Center aligned PWM operation
    PTCONbits.PTEN = 1;                 // Start PWM
    return;
}
//Initialize I2C mode
void I2C_init(void){
	I2CCON = 0x8000;	// Enable I2C1 slave module
	I2CADD = 0x77;          // Initialised 7-bits I2C slave address

	//IFS1 = 0;
        IFS0bits.SI2CIF = 0;    // I2C Transfer Complete Interrupt Flag Status bit
                                // 1 = Interrupt request has occurred
                                // 0 = Interrupt request has not occurred
	RAMPtr = &RAMBuffer[0]; // Set the RAM pointer and points to beginning of RAMBuffe
	IEC0bits.SI2CIE = 1;    // I2C Transfer Complete Interrupt Enable bit
                                // 1 = Interrupt request enabled
                                // 0 = Interrupt request not enabled
}
//Initialize ADC mode
void ADC_init(void){
    TRISB = 0xFFFF;             // Port B all inputs
    ADPCFG = 0x0000;            // All 16 PORTB pins are analog inputsb
    ADCON1 = 0;                 // Manually clear SAMP to end sampling, start conversion
    ADCON2 = 0;                 // Voltage reference from AVDD and AVSS
    ADCON3bits.ADCS = 30;       // A/D Conversion Clock Select (Tcy/2)*(ADCS+1) = Tcy
                                // 30 * 33 = 990 ns(1 MHz)
                                // Our max PWM frequency after is 120 Hz so 1 MHz is way to fast
    ADCON1bits.ADON = 1;        // Turn ADC ON
}

//PWM main loop
void __attribute__((interrupt, no_auto_psv)) _PWMInterrupt(void){

    PWMCON2bits.UDIS = 1;                                   //Updates from duty cycle and period buffer registers are disabled

    Phase += Delta_Phase;                                   // Accumulate Delta_Phase in Phase variable
    Phase_Offset1 = _0_DEGREES;                             // Add proper value to phase offset (0 degree)
    Multiplier = sinetable[(Phase + Phase_Offset1) >> 10];  // Take sinetable info

    asm("MOV _Multiplier, W4");                             // Load first multiplier
    asm("MOV _PTPER, W5");                                  // Load second multiplier
    asm("MOV #_Result, W0");                                // Load W0 with the address of Result
    asm("MPY W4*W5, A");                                    // Perform Fractional multiplication
    asm("SAC A, [W0]");                                     // Store multiplication result in var Result

    PDC1 = Result + PTPER;                                          // The duty cycle of first PWM

    Phase_Offset2 = _120_DEGREES;                           // Add proper value to phase offset (120 degree)
    Multiplier = sinetable[(Phase + Phase_Offset2) >> 10];  // Take sinetable info

    asm("MOV _Multiplier, W4");                             // Load first multiplier
    asm("MOV _PTPER, W5");                                  // Load second multiplier
    asm("MOV #_Result, W0");                                // Load W0 with the address of Result
    asm("MPY W4*W5, A");                                    // Perform Fractional multiplication
    asm("SAC A, [W0]");                                     // Store multiplication result in var Result
    
    PDC2 = Result + PTPER;                                          // The duty cycle of second PWM

    Phase_Offset3 = _240_DEGREES;                           // Add proper value to phase offset (240 degree)
    Multiplier = sinetable[(Phase + Phase_Offset3) >> 10];  // Take sinetable inf

    asm("MOV _Multiplier, W4");                             // Load first multiplier
    asm("MOV _PTPER, W5");                                  // Load second multiplier
    asm("MOV #_Result, W0");                                // Load W0 with the address of Result
    asm("MPY W4*W5, A");                                    // Perform Fractional multiplication
    asm("SAC A, [W0]");                                     // Store multiplication result in var Result

    PDC3 = Result + PTPER;                                          // The duty cycle of third PWM

    PWMCON2bits.UDIS = 0;                                   //Updates from duty cycle and period buffer registers are enabled
    IFS2bits.PWMIF = 0;                                     //Clear PWM Interrupt Request Flags

}
//I2C main loop
void __attribute__((interrupt,no_auto_psv)) _SI2CInterrupt(void){

	unsigned char Temp;                                     // Used for dummy read

	if((I2CSTATbits.R_W == 0)&&(I2CSTATbits.D_A == 0))	// Address matched
		{
			Temp = I2CRCV;                          // Dummy read
			AddrFlag = 1;                           // Next byte will be address
		}
	else if((I2CSTATbits.R_W == 0)&&(I2CSTATbits.D_A == 1))	// Check for data
		{
			if(AddrFlag)                            // If Address Flag
			{
				AddrFlag = 0;                   // Clear Address Flag
				DataFlag = 1;                   // Next byte is data
				RAMPtr = RAMPtr + I2CRCV;       // Match the Sub-slave address
			}
			else if(DataFlag)
			{
				*RAMPtr = (unsigned char)I2CRCV;// Store data into RAM
				DataFlag = 0;                   // Clear Data flag
				RAMPtr = &RAMBuffer[0];         // Reset the RAM pointer
			}
		}
        else if((I2CSTATbits.R_W == 1)&&(I2CSTATbits.D_A == 0)) // Check for read
	{
		Temp = I2CRCV;                                  // Dummy read
		I2CTRN = *RAMPtr;                               // Read data from RAM & send data to I2C master device
		I2CCONbits.SCLREL = 1;                          // Release SCL1 line
		while(I2CSTATbits.TBF);                         // Wait till all
		RAMPtr = &RAMBuffer[0];                         // Reset the RAM pointer
	}

	_SI2CIF = 0;                                            // Clear I2C1 Slave interrupt flag
}
//read value from ADC
int read_ADC(int pin, int delay){

    ADCHSbits.CH0SA = pin;      // Select ANx
    ADCON1bits.SAMP = 1;        // Start sampling
    __delay32(delay);           // delay*33ns delay at 30 MIPS
    ADCON1bits.SAMP = 0;        // Start Converting
    while (!ADCON1bits.DONE);   // Wait until A/D conversion is done
                                // Should take 12 * Tad = 12 * 1 = 12 us
    return ADCBUF0;             // Return value
}
//Calculate RMS value
void RMS(int *diff_h, int *diff_l, int index1, int index2){
    
    int sum = 0;                                                        // Initialize sum to zero
    int mean, root;                                                     // Declare mean and root
    int i;                                                              // Declare index

    for(i=0; i<64; i++){                                                // Calculate square sum
        sum += (diff_h[i] - diff_l[i]) * (diff_h[i] - diff_l[i]);
    }
    mean = sum/64;                                                      // Calculate mean value with 64 entrys
    root = sqrt(mean);                                                  // Calculate root base on mean

    RAMBuffer[index1] = root >> 8;                                      // Put higher 8-bit into the 1 RAM register
    RAMBuffer[index1] = root - (root >> 8);                             // Put lower 8-bit into the 1 RAM registers
    
}
int main() {

    int i;                                                  // Declare index
    int time_unit;                                          // Declare 1 time delay
    int p1_v_h[64], p1_v_l[64], p1_c_h[64], p1_c_l[64],     // Declare all differential input with 64 entrys
        p2_v_h[64], p2_v_l[64], p2_c_h[64], p2_c_l[64],
        p3_v_h[64], p3_v_l[64], p3_c_h[64], p3_c_l[64],
        dc_h[64], dc_l[64];
    
    I2C_init();                                             // Initialize I2C mode
    PWM_init();                                             // Initialize PWM mode
    ADC_init();                                             // Initialize ADC mode

    RAMBuffer[frequency] = 17;                              // Initialize frequency to 17 Hz

    while(1){

        Delta_Phase = (RAMBuffer[frequency] * 65536 / FPWM);// Calculate delta phase base on frequency

        time_unit = 2319002/RAMBuffer[frequency];           // Calculate ADC sampling time base on frequency

        for(i=0; i<64; i++){                                // Get the all 14 differential inputs
            p1_v_h[i] = read_ADC(0, time_unit);
            p1_v_l[i] = read_ADC(0, time_unit);
            p1_c_h[i] = read_ADC(0, time_unit);
            p1_c_l[i] = read_ADC(0, time_unit);
            p2_v_h[i] = read_ADC(0, time_unit);
            p2_v_l[i] = read_ADC(0, time_unit);
            p2_c_h[i] = read_ADC(0, time_unit);
            p2_c_l[i] = read_ADC(0, time_unit);
            p3_v_h[i] = read_ADC(0, time_unit);
            p3_v_l[i] = read_ADC(0, time_unit);
            p3_c_h[i] = read_ADC(0, time_unit);
            p3_c_l[i] = read_ADC(0, time_unit);
            dc_h[i] = read_ADC(0, time_unit);
            dc_l[i] = read_ADC(0, time_unit);
        }
                                                            // Calculate all RMS value and store into the RAM
        RMS(p1_v_h,  p1_v_l, PWM1_voltage_h, PWM1_voltage_l);
        RMS(p1_c_h,  p1_c_l, PWM1_current_h, PWM1_current_l);
        RMS(p2_v_h,  p2_v_l, PWM2_voltage_h, PWM2_voltage_l);
        RMS(p2_c_h,  p2_c_l, PWM2_current_h, PWM2_current_l);
        RMS(p3_v_h,  p3_v_l, PWM3_voltage_h, PWM3_voltage_l);
        RMS(p3_c_h,  p3_c_l, PWM3_current_h, PWM3_current_l);
        RMS(dc_h,  dc_l, DC_voltage_h, DC_voltage_l);

    };

    return 0;
}

