#include <stdio.h>
#include <stdlib.h>

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
#include <i2c.h>

_FOSC(CSW_FSCM_OFF & FRC_PLL16);

#define FCY     29480000    //7.37M Hz * 16 / 4 = 29.48M Hz
#define Fscl    400000      //SCL is 400k Hz
#define config1 (unsinged int)(((FCY/Fscl) - (FCY/10000000)) - 1)   //I2CBRG = 69.752
#define config2 0x8000      //I2CCON = 0b1000,0000,0000
#define address 0x77        //define slave address is 0x77

//unsigned char RAMPtr[8];			//Pointer to RAM memory locations

unsigned char RAMBuffer[256];	//RAM area which will work as EEPROM for Master I2C device
unsigned char *RAMPtr;			//Pointer to RAM memory locations
int AddrFlag = 0;	//Initlize AddFlag
int DataFlag = 0;	//Initlize DataFlag

/*****************************************************************
		Init I2C1 Bus
*****************************************************************/
void i2c_init(void)
{
        LATD = 0;
        TRISD = 0b11111110;
	I2CCON = 0x8000;	//Enable I2C1 module
	I2CADD = 0x50;			// 7-bit I2C slave address must be initialised here.

	IFS1=0;
	RAMPtr = &RAMBuffer[0];	//set the RAM pointer and points to beginning of RAMBuffe
	_SI2CIE = 1;
}

/*
Function Name: SI2C1Interrupt
Description : This is the ISR for I2C1 Slave interrupt.
Arguments	 : None
*/
void __attribute__((interrupt,no_auto_psv)) _SI2CInterrupt(void)
{
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

int main(int argc, char** argv) {

    i2c_init();
    _LATD0 = 1;
    while(1){
        if(RAMPtr[0] == '0' )   _LATD0 = 0;
        if(RAMPtr[0] == '1' )   _LATD0 = 1;
    }

    return (EXIT_SUCCESS);
}

