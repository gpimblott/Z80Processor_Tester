/* 
 * File:   Z80Tester
 * Author: G.Pimblott
 *
 * Created on 30 December 2020, 21:52
 */
// CONFIG
#pragma config FOSC = EXTRCCLK  // Oscillator Selection bits (RC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, RC on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdbool.h>

/*
 * Defines
 */
#define CLOCK_DELAY 10

void initHW( void ) {   
    ADCON0 = 0x00;
    ADCON1 = 0x00;
    
    ADCON0bits.ADON = 0;
    
    CM1CON0 = 0x00;
    CM2CON0 = 0x00;
    CM2CON1 = 0x00;
    
    CCP1CON = 0x00;
   
    INTCON = 0x0;
    
    /**
     *  Define which pins will be Digital / analog
     * 0 == digital, 1 == analog
     */
    ANSEL = 0x0;    // set to digital I/O (not analog)
    ANSELH = 0x0;   // Set to digital I/O (not analog)
    //ANSELHbits.ANS10 = 0;
    
    //IOCB4 = 0;
    
    /**
     * Define the direction of our pins
     * 0 == output, 1 == input
     *
     * For the Z80 tester
     * RC0 : Clock output
     */ 
    TRISA = 0b00000000;    // All PORTA as output

    //TRISB4 = 0b00010000;     // RB4 input for reset button - issue ?
    TRISBbits.TRISB4 = 1;
    
    TRISC = 0b00000000;    // All PORTC as output
    
    /**
     * Pull-ups
     */
    WPUB4 = 1;
    
    /**
     * Default state for pins
     */
    PORTC &= 0b00000000;    // Default all off
    
    RC0 = 0;    // Clock LED
    RC1 = 0;    // Reset LED
}

_Bool clockOn = true;

/*
 * 
 */
int main(int argc, char** argv) {
    initHW();
 
    while (1) {

        PORTC = 0b00000001;        
        __delay_ms(100);
        PORTC = 0b00000010;
        __delay_ms(100);

    }
    return 0;
}

