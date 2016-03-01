/* 
 * File:   Table-infini-main.c
 * Author: Mickaël Tansorier
 *
 * Created on 9 mars 2015, 18:44
 * 
 * License CC-BY-NC-SA
 * https://creativecommons.org/licenses/by-nc-sa/3.0/fr/
 */

/* CONFIGURATION */
#pragma config CP = ON          // Flash Program Memory Code Protection bit
#pragma config CCPMX = RB3      // CCP1 Pin Selection bit
#pragma config DEBUG = OFF      // In-Circuit Debugger Mode bit 
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CPD = ON         // Data EE Memory Code Protection bit 
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (desactiver si fonctionne en 3V)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit 
#pragma config PWRTE = ON       // Power-up Timer Enable bit (choix reset inten))
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config FOSC = INTOSCIO

#include <pic16f87.h>
#include <xc.h>
#include <htc.h>

#define _XTAL_FREQ 8000000 //set your internal(or)external oscillator speed


/* ALIAS */
#define RED   PORTBbits.RB1
#define GREEN PORTBbits.RB2
#define BLUE  PORTBbits.RB3

#define LED1  PORTBbits.RB4
#define LED2  PORTBbits.RB5
#define LED3  PORTBbits.RB6
#define LED4  PORTBbits.RB7
#define LED5  PORTAbits.RA0
#define LED6  PORTAbits.RA1
#define LED7  PORTAbits.RA2
#define LED8  PORTAbits.RA3
#define LED9  PORTAbits.RA4
#define LED10 PORTAbits.RA6

/* FONCTIONS */
void init_oscillator (void);
void init_timer0     (void);
void init_int0       (void);
void interrupt ISR   (void);
void change_color_toutes_leds    (void);

/* VARIABLES */
volatile unsigned char PWM_COUNTER = 0; // 0 to 100 : nb ntrerrupt during PWM cycle
volatile unsigned char duty_RED = 0;    // 0 to 100
volatile unsigned char duty_GREEN = 0;  // 0 to 100
volatile unsigned char duty_BLUE = 0;   // 0 to 100
volatile unsigned char mode = 0;        // 0 to 4   : 4 differents mode

/*
 * MAIN
 */
int main(void) {
    
    init_oscillator();
    
    /* DEFINE PORTs */
    TRISA=0x00;
    TRISB=0x01; // int0
    
    /* INIT */
    init_timer0();
    init_int0();

    /* INTERRUPT */
    INTCONbits.GIE = 1; // Enables all unmasked interrupts
    
    /* INIT PORT */
    PORTA=0x00;
    PORTB=0x00;

    while(1)
    {
                
        if(mode==1 || mode==0)
        {
            change_color_toutes_leds();
        }
        if(mode==2 || mode==0){
            duty_RED   = 100;
            duty_GREEN = 0;
            duty_BLUE  = 0;
            LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = 1;
            __delay_ms(1000);
        }
        
        if(mode==3 || mode==0){
            duty_RED   = 0;
            duty_GREEN = 100;
            duty_BLUE  = 0;
            LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = 1;
            __delay_ms(1000);
        }
        
        if(mode==4 || mode==0){
            duty_RED   = 0;
            duty_GREEN = 0;
            duty_BLUE  = 100;
            LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = 1;
            __delay_ms(1000);
        }
        
    }
    return 0;
}

void init_oscillator(void){
    OSCCONbits.IRCF = 0b111; // 8MHz
    OSCCONbits.SCS = 0b00;   // Oscillator mode defined by FOSC
    OSCCONbits.IOFS = 1;     // Frequency is stable
}

void init_timer0(void)
{
    /* interruption timer */
    OPTION_REGbits.T0CS = 0; // Internal instruction cycle clock (CLKO)
    OPTION_REGbits.PSA  = 0; // Prescaler is assigned to the Timer0 module
    
    /* T=(4/Fosc)*Presc*(2^8-TMR0)
     * T=(4/8000000)*8*(256 - (256 - 25))
     * T=100us
     */
    OPTION_REGbits.PS = 0b010; // Prescaler Rate Select bits
    TMR0 = 256-25;
  
    INTCONbits.TMR0IE = 1; // Enables the TMR0 interrupt
    INTCONbits.TMR0IF = 0; // Enables the TMR0 interrupt
}

void init_int0(void){
    OPTION_REGbits.nRBPU  = 0; //PORTB Pull-up Enable bit
    OPTION_REGbits.INTEDG = 0; //Interrupt Edge Select bit 1=front montant, 0= front descenndant
    INTCONbits.INT0IE = 1; //Enables the RB0/INT external interrupt
    INTCONbits.INT0IF = 0;
}

void interrupt ISR(void) 
{
    /* Check if it is TMR0 Overflow ISR */
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF){

        if(PWM_COUNTER < duty_RED) {RED = 1;}
        else{RED=0;}

        if(PWM_COUNTER < duty_GREEN){GREEN = 1;}
        else{GREEN=0;}

        if(PWM_COUNTER < duty_BLUE){BLUE = 1;}
        else{BLUE=0;}
        
        PWM_COUNTER++;
        if(PWM_COUNTER>=100){
            PWM_COUNTER=0;
        }
        
        TMR0 = 255-25;
        INTCONbits.TMR0IF=0; //reactive l'interruption
    }
    /* changement de mode */
    if(INTCONbits.INT0IE && INTCONbits.INT0IF)
    {
        
        mode++;
        if(mode>4) mode=0;
        if(mode==0){LED1=LED2=LED3=1;}
        else if(mode==1){LED1=1;LED2=LED3=0;}
        else if(mode==2){LED2=1;LED1=LED3=0;}
        else if(mode==3){LED1=LED2=1;LED3=0;}
        else if(mode==4){LED3=1;LED2=LED1=0;}
        else{LED3=LED1=1;LED2=0;}
        __delay_ms(250);
        INTCONbits.INT0IF=0; // ennable interrupt
        
    }
}


/*
 * CHANGE LEDS COLOR 
 */
void change_color_toutes_leds(void)
{
    /* differents states */
    //--- --- ___ ___ ___ ---
    //___ --- --- --- ___ ___
    //___ ___ ___ --- --- ---
    
    // initial state
    duty_RED=100;
    duty_GREEN=0;
    duty_BLUE=0;
    // Leds on
    LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = 1;
    
    signed char i;
    // up green
    for(i=0;i<=100;i++){
        duty_GREEN=i;
        __delay_ms(10);
    }
    if(mode!=1 && mode!=0)return;
    // down red
    for(i=100;i>=0;i--){
        duty_RED=i;
        __delay_ms(10);
    }
    if(mode!=1 && mode!=0)return;
    // up blue
    for(i=0;i<=100;i++){
        duty_BLUE=i;
        __delay_ms(10);
    }
    if(mode!=1 && mode!=0)return;
    // down green
    for(i=100;i>=0;i--){
        duty_GREEN=i;
        __delay_ms(10);
    }
    if(mode!=1 && mode!=0)return;
    // up red
    for(i=0;i<=100;i++){
        duty_RED=i;
        __delay_ms(10);
    }
    if(mode!=1 && mode!=0)return;
    // down blue
    for(i=100;i>=0;i--){
        duty_BLUE=i;
        __delay_ms(10);
    }
}

