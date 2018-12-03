/**
  ******************************************************************************
  *	@mainpage
  * ATM simulator
  * @file    main.c
  * @author  Jan Sýkora and Jan Mrkos, Brno University of Technology, Czech Republic
  * @version V1.1
  * @date    Dec 2, 2018
  * @brief   BMPT semester project.
  * @note    ATM simulator with 4x4 matrix membrane keyboard
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "settings.h"
#include <avr/io.h>
#include <stdlib.h>         /* itoa() function */
#include "lcd.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

/* Constants and macros ------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
/**
 *  @brief Startup initialize.
 */
void setup(void);

/* Global variables ----------------------------------------------------------*/
char lcd_string[5];
char lcd_string1[5];
char lcd_string2[5];
char lcd_string3[5];
char lcd_string4[5];

volatile    uint8_t i;
volatile    uint16_t val=0;		/* LCD show variable */
volatile    uint16_t value=0;
volatile    uint8_t temp=0;
volatile    uint8_t att=0;
volatile    uint8_t set=0;

volatile    uint8_t val1=0;
volatile    uint8_t val2=0;
volatile    uint8_t val3=0;
volatile    uint8_t val4=0;
volatile    uint8_t val5=0;
volatile    uint8_t val6=0;
volatile    uint8_t val7=0;
volatile    uint8_t val8=0;

/* FSM states	----------------------------------------------------------*/
typedef enum {
START,
VALID,
PASS,
FAIL,
} type_state;
type_state  current_state = START;   

/* Functions -----------------------------------------------------------------*/
/**
  * @brief Main function.
  */
int main(void)
{	/* Initializations */	
    setup();  
	/* All interrupt enable */  
    sei();
	lcd_clrscr();   	
	/* Forever loop */		
    while (1) {		
	}
    return 0;
}

void setup(void)
{	/*Settings of I/O PORTS */
    DDRD &= ~_BV(PD2);		
    DDRD &= ~_BV(PD3);
    DDRC &= ~_BV(PC0);
    DDRC &= ~_BV(PC2);
	
	DDRC |= _BV(PC5);
	DDRC |= _BV(PC4);
	DDRC |= _BV(PC3);	
	/* Setting High state to row PORTS */
    PORTD |= _BV(PD2);		
    PORTD |= _BV(PD3);
    PORTC |= _BV(PC0);
    PORTC |= _BV(PC2);
	/* Startup state of columns  for reading from keyboard matrix */
    PORTC |= _BV(PC3);
    PORTC |= _BV(PC4);
    PORTC &= ~_BV(PC5);	
	/* Clock prescaler 1 => overflows every 4 ms */  
    TCCR1B  |= _BV(CS10);	
	/* Timer1 overflow interrupt enable	*/
    TIMSK1 |= _BV(TOIE1);	
    /* Initialize display and select type of cursor */
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);  
    /* Clear display and set cursor to home position */
    lcd_clrscr();
}

/**
  * @brief TC1 interrupt - Repeat on every timer1 overflow
  */
ISR(TIMER1_OVF_vect)
{	lcd_clrscr();
	switch (current_state) {
		/**
			* @brief START STATE - Startup PIN setup and then verifying it
			 */
		/* Setup and test of correct PIN code */
		case START:
		{		
			if (set==0)
			lcd_puts("SET PIN");
			else
			lcd_puts("VERIFY PIN ");
			
			/* Reading from 4x4 matrix keyboard */
			/* Every cycle the PIN number is shown and stored, in real life ATM shows "*" symbol instead
				We use high state of rows and low state of columns to determine which number was pressed */
			/* For PIN position shift we multiply keyboard value by 10^i  */	
			for(i=1;i<5;i++) {
				/* Marker position setup to end of phrase "SET PIN" */		
				lcd_gotoxy(i+11,0);
				/* Reset val value to zero */ 
				val=0;				
				/* Read and print of specific number, store value if button is pressed
					 and position shift for next number */
				if(bit_is_clear(PIND,2)) {			
					lcd_puts("1");
					val=pow(10,i);
					temp=1;
					_delay_ms(300);
				}
				if(bit_is_clear(PIND,3)) {			
					lcd_puts("2");
					val=pow(10,i);
					temp=2;
					_delay_ms(300);
				}
				if(bit_is_clear(PINC,0)) {			
					lcd_puts("3");
					_delay_ms(300);
					val=pow(10,i);
					temp=3;
				}	
				/* Port setup for second column numbers */
				PORTC |= _BV(PC3);
				PORTC &= ~ _BV(PC4);
				PORTC |= _BV(PC5);

				if(bit_is_clear(PIND,2)) {			
					lcd_puts("4");
					val=pow(10,i);
					temp=4;
					_delay_ms(300);
				}
				if(bit_is_clear(PIND,3)) {			
					lcd_puts("5");
					val=pow(10,i);
					temp=5;
					_delay_ms(300);
				}
				if(bit_is_clear(PINC,0)) {			
					lcd_puts("6");
					val=pow(10,i);
					temp=6;
					_delay_ms(300);
				}
				/* Port setup for third column numbers */
				PORTC &= ~ _BV(PC3);
				PORTC |= _BV(PC4);
				PORTC |=_BV(PC5);

				if(bit_is_clear(PIND,2)) {
					lcd_puts("7");
					val=pow(10,i);
					temp=7;
					_delay_ms(300);
				}
				if(bit_is_clear(PIND,3)) {			
					lcd_puts("8");
					val=pow(10,i);
					temp=8;
					_delay_ms(300);
				}
				if(bit_is_clear(PINC,0)) {
					lcd_puts("9");
					val=pow(10,i);
					temp=9;
					_delay_ms(300);
				}
				else {		
					_delay_ms(300);
				}
				/* Store PIN code to variables  */
				if (set==1) {		
					if (i==1)
					val1=temp;
					if (i==2)
					val2=temp;
					if (i==3)
					val3=temp;
					if (i==4)
					val4=temp;
				}
				if (set==0) {		
					if (i==1)
					val5=temp;
					if (i==2)
					val6=temp;
					if (i==3)
					val7=temp;
					if (i==4)
					val8=temp;
				}
				/* Holding position on one place waiting for next number */ 
				if(val<10) {		
					i=i-1;
				}
				/* Back to default setup column numbers */
				PORTC |= _BV(PC3);
				PORTC |= _BV(PC4);
				PORTC &= ~_BV(PC5);

			} 
			_delay_ms(2000);			
			set=1;	
			current_state = VALID;
			break;
		}
		
		case VALID:
		{
				if(val1==0) {			
					lcd_puts("PIN SET");
					lcd_gotoxy(0,1);
					lcd_puts("PRESS: A");
					_delay_ms(300);
				}
				/* PIN code is correct */
				if((val5==val1)&&(val6==val2)&&(val7==val3)&&(val8==val4)) {			
					lcd_puts("PIN OK");
					att=0;
					_delay_ms(20000);
				}
			
				if((val1>0)&&(val2>0)&&(val3>0)&&(val4>0)) {			
					current_state = PASS;
				}

				if(bit_is_clear(PINC,5)) {			
					if(bit_is_clear(PINC,2)) {			
						current_state = START;
					}
				}
		break;
		}

		case PASS:
		{	/* 3 attempts counter */
			att++;
			if (att==3) {			
				current_state = FAIL;
				att=0;
			}
			/* Error message with remaining attempts left if wrong PIN code is set */
			else {			
				current_state = START;
				lcd_clrscr();
				lcd_puts("WRONG");
				lcd_gotoxy(0,1);
				lcd_puts("LEFT");
				itoa((3-att),lcd_string3,10);
				lcd_puts(lcd_string3);
				_delay_ms(1000);
			}		
		break;
		}

		case FAIL:
		{	/* Destruction of ATM if PIN code is wrong for third time + explosion countdown */
			uint8_t tim;
			lcd_clrscr();
			lcd_puts("3x WRONG PIN");
			for(tim=9;tim>0;tim--) {			
				lcd_gotoxy(0,1);
				lcd_puts("AUTODESTRUCTION:");
				itoa(tim,lcd_string3,10);
				lcd_puts(lcd_string3);
				_delay_ms(1000);
			}
			current_state = START;
			break;
		}	
	}
}
