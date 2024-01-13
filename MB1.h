
// CONFIG1
#pragma config FOSC = INTOSC	// Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF		// Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF		// Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON		// MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF			// Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF		// Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF		// Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF	// Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON		// Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON		// Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF		// Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF		// PLL Enable (4x PLL enabled)
#pragma config STVREN = ON		// Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO		// Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config DEBUG = OFF		// In-Circuit Debugger Mode (In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins)
#pragma config LVP = OFF		// Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 8000000

void main_init(void);
char ASCII_TH(int Value, char bit);
void bit_Value(void);
void EEPROM_write(int *eAddr, int *eDate, char *cData);

#define SDA		PORTAbits.RA5
#define T_SDA	TRISAbits.TRISA5

#define SCL		PORTAbits.RA4
#define T_SCL	TRISAbits.TRISA4

#define DI		PORTAbits.RA0
#define T_DI	TRISAbits.TRISA0

#define RO		PORTAbits.RA1
#define T_RO	TRISAbits.TRISA1
