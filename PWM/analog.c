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
#include <libpic30.h>
#include <p30f4012.h>

void configure_pins();
unsigned int read_analog_channel(int n);

int main()
{
    long int n;

    // Set up which pins are which
    configure_pins();

    while(1)
    {
        // Read the analog channel. The result is an
        // integer between 0 and 1023 inclusive.
        n = read_analog_channel(0);

        // LED on
        _LATD0 = 1;
        __delay32(30*n); // m us

        // LED off
        _LATD0 = 0;
        __delay32(30*(1023 - n)); // (1023-n) us
    }

    return 0;
}

void configure_pins()
{
    // Configure digital I/O
    LATD = 0;
    TRISD = 0b11111110;

    // Configure analog inputs
    TRISB = 0x01FF;      // Port B all inputs
    ADPCFG = 0xFF00;     // Lowest 8 PORTB pins are analog inputs
    ADCON1 = 0;          // Manually clear SAMP to end sampling, start conversion
    ADCON2 = 0;          // Voltage reference from AVDD and AVSS
    ADCON3 = 0x0007;     // Manual Sample, ADCS=5 -&gt; Tad = 3*Tcy = 0.1us
    ADCON1bits.ADON = 1; // Turn ADC ON
}

// This function reads a single sample from the specified
// analog input. It should take less than 2.5us if the chip
// is running at about 30 MIPS.
unsigned int read_analog_channel(int channel)
{
    ADCHS = channel;          // Select the requested channel
    ADCON1bits.SAMP = 1;      // start sampling
    __delay32(30);            // 1us delay @ 30 MIPS
    ADCON1bits.SAMP = 0;      // start Converting
    while (!ADCON1bits.DONE); // Should take 12 * Tad = 1.2us
    return ADCBUF0;
}