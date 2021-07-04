/* 
 * File:   main.c
 * Author: G.Pimblott
 *
 */
// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (RC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, RC on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define _XTAL_FREQ 8000000 // 8Mhz Crystal
//#define _XTAL_FREQ 20000000 // 20MHz crystal

#include <xc.h>

// Simple
#define ON          1
#define OFF         0

#define BTN_DOWN    0
#define BTN_UP      1

#define HIGH        1
#define LOW         0

// Define output macros
#define CLK_OUT     PORTCbits.RC0
#define INST_OUT    PORTCbits.RC1
#define RESET_OUT   PORTCbits.RC2

// Define button macros
#define STOP_START_BTN      PORTAbits.RA0
#define RESET_BTN           PORTAbits.RA1

// Constants
#define RESET_CLK_CYCLES    10


volatile unsigned char m1Count = 0;
volatile unsigned char clockRunning = OFF;
volatile unsigned char resetCount = 0;

/**
 * Setup timer 2 to generate on interrupt
 */
void init_interrupts(void) {
    // Clear the timer bits
    TMR2 = 0;
    PR2 = 0xff;

    // Set the Clock pre-scaler
    // 00 = Pre-scaler is 1
    // 01 = Pre-scaler is 4
    // 1x = Pre-scaler is 16
    T2CONbits.T2CKPS0 = 0;
    T2CONbits.T2CKPS1 = 1;

    // Set output post-scaler
    // 0000 = 1:1 Post-scaler
    // 0001 = 1:2 Post-scaler
    // 0010 = 1:3 Post-scaler
    // 0011 = 1:4 Post-scaler
    // 0100 = 1:5 Post-scaler
    // 0101 = 1:6 Post-scaler
    // 0110 = 1:7 Post-scaler
    // 0111 = 1:8 Post-scaler
    // 1000 = 1:9 Post-scaler
    // 1001 = 1:10 Post-scaler
    // 1010 = 1:11 Post-scaler
    // 1011 = 1:12 Post-scaler
    // 1100 = 1:13 Post-scaler
    // 1101 = 1:14 Post-scaler
    // 1110 = 1:15 Post-scaler
    // 1111 = 1:16 Post-scaler
    T2CONbits.TOUTPS0 = 1;
    T2CONbits.TOUTPS1 = 1;
    T2CONbits.TOUTPS2 = 1;
    T2CONbits.TOUTPS3 = 1;

    PIE1bits.TMR2IE = 1;    // Enable Timer 2 Interrupts
    T2CONbits.TMR2ON = 1;   // Turn the timer On

    /**
     * Interrupt on change
     */
//    IOCAbits.IOCA0 = 1;     //Enabled RA0 Interrupt-On-Change
//    INTCONbits.RABIE = 1;   //Enabled Interrupt-On-Change Interrupt
//    INTCONbits.RABIF = 0;   // Clear the interrupt bit
    
    /**
     * Global interrupt enable
     */
    INTCONbits.PEIE = 1;    // Enable the peripheral interrupts (which includes timer1)
    INTCONbits.GIE = 1;     // Enable global interrupts
    
    
}

/**
 * Setup timer 1 to generate an interrupt
 */
//void init_timer1(void) {
//    // Clear the timer bits
//    TMR1H = TMR1L = 0;
//
//    // Set pre-scaler to 0b10 == 4
//    // T1CKPS<1:0>: Timer1 Input Clock Pre-scale Select bits
//    // 11 = 1:8 Pre-scale Value
//    // 10 = 1:4 Pre-scale Value
//    // 01 = 1:2 Pre-scale Value
//    // 00 = 1:1 Pre-scale Value
//    T1CONbits.T1CKPS1 = 0;
//    T1CONbits.T1CKPS0 = 0;
//
//    T1CONbits.TMR1CS = 0; // Internal oscillator
//    PIE1bits.TMR1IE = 1; // Fire overflow interrupt 1
//    T1CONbits.TMR1ON = 1; // Turn the timer On
//
//    INTCONbits.PEIE = 1; // Enable the peripheral interrupts (which includes timer1)
//    INTCONbits.GIE = 1; // Enable global interrupts
//
//}

void initHW(void) {
    ANSEL = 0;          //Initialise A/D ports off
    ANSELH = 0;         //Initialise A/D ports off
    CM1CON0 = 0;        //Initialise comp 1 off
    CM2CON0 = 0;        //Initialise comp 2 off
    
    //TRISA = 0b00000100;     // RA2 as an input
    //TRISC = 0b00000000;     // All PORTC pins as outputs
    //TRISA = 0b00000111;     // A0-2 as inputs

    // Setup outputs C0 & C1
    TRISC0 = 0; // Clock signal
    TRISC1 = 0; // Instruction (M1) ?)
    TRISC2 = 0; // Reset line
    TRISC3 = 0; // Test output

    // Setup inputs
    TRISA0 = 1; // Start/Stop
    TRISA1 = 1; // Reset button
    TRISA2 = 1;

    // Set the default values

    CLK_OUT = LOW;      // Start the clock low
    INST_OUT = LOW;     // Start the instruction monitor low
    RESET_OUT = HIGH;   // Default reset line to HIGH (It's active low))
    
    //RC3 = LOW;

    init_interrupts();
}

/**
 * Interrupt handler routine
 */
__interrupt(high_priority) void myisr() {

//    if( INTCONbits.RABIF == 1 ) {
//        RC3 = HIGH;
//        
//        INTCONbits.RABIF = 0;
//    }
    
    // was interrupt was fired by timer1
    if (PIR1bits.TMR2IF == 1) {

        if (clockRunning == ON || resetCount != 0) {

            CLK_OUT = ~CLK_OUT; // Flip the clock output

            // Keep track of the instruction cycles
            if (m1Count++ == 4) {
                INST_OUT = ~INST_OUT;
                m1Count = 0;
            }

            // Check if we have finished a reset cycle
            if (resetCount > 0) {
                if (--resetCount == 0) {
                    RESET_OUT = HIGH;
                }
            }
        }

        // Re-enable timer2
        PIR1bits.TMR2IF = 0;
    }
}

/**
 * 
 */
void startReset() {
    RESET_OUT = LOW;
    resetCount = RESET_CLK_CYCLES;
}

/*
 * 
 */
int main(int argc, char** argv) {
    initHW();

    // Force a reset
    //startReset();

    // Loop forever
    while (1) {

        if (STOP_START_BTN == BTN_DOWN) {
            __delay_ms(100); // Hack to remove bounce

            // If it's on turn the clock off
            if (clockRunning == ON) {
                clockRunning = OFF;

                // Change the outputs
                CLK_OUT = 0;
                INST_OUT = 0;

                // REset counters
                m1Count = 0;
                resetCount = 0;
            } else {
                clockRunning = ON;
            }
        }

        if (RESET_BTN == BTN_DOWN) {
            __delay_ms(100); // Hack to remove bounce
            startReset();
        }
    }
    return 0;
}

