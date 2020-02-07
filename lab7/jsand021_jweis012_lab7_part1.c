/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_

#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn(){
	// avr timer/counter controller register TCCR1
	TCCR1B = 0x0B;

	// avr output compare register OCR1A
	OCR1A = 125;

	// avr timer interrupt mask register
	TIMSK1 = 0x02;

	// initialize avr counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	// enable global interrupts
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


enum States {init, wait, pressA0, holdA0, pressA1, holdA1, reset} state;
typedef enum bool {false, true} bool;
unsigned char cnt;
unsigned char tmp;




void tick();

void LCD_switch(char tmp){
	switch(tmp){
		case 0: LCD_DisplayString(1, "0"); break;
		case 1: LCD_DisplayString(1, "1"); break;
		case 2: LCD_DisplayString(1, "2"); break;
		case 3: LCD_DisplayString(1, "3"); break;
		case 4: LCD_DisplayString(1, "4"); break;
		case 5: LCD_DisplayString(1, "5"); break;
		case 6: LCD_DisplayString(1, "6"); break;
		case 7: LCD_DisplayString(1, "7"); break;
		case 8: LCD_DisplayString(1, "8"); break;
		case 9: LCD_DisplayString(1, "9"); break;
	}
}

int main (void)
{
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	LCD_ClearScreen();
	LCD_DisplayString(1, "0");
	
	state = init;
	cnt = 0;
	tmp = 0;

	TimerSet(100);
	TimerOn();
	while (1) {
		tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}




void tick(){
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = (~PINA & 0x02) >> 1;


	switch(state){
		case init:
			tmp = 0x00;
			state = wait;
			break;
		case wait:
			if(A0 && A1){
				state = reset;
			}
			else if(A0){
				state = pressA0;
			}
			else if(A1){
				state = pressA1;
			}
			else{
				state = wait;
			}
			break;
		case holdA0:
		if(A1){
			state = reset;
		}
		else if(A0){
			state = holdA0;
		}
		else if(!A0){
			state = wait;
		}
		break;
		case pressA0:
		if(A1){
			state = reset;
		}
		else if(A0){
			state = holdA0;
		}
		else if(!A0){
			
			state = wait;
		}
		break;
		case holdA1:
		if(A0){
			state = reset;
		}
		else if(A1){
			state = holdA1;
		}
		else if(!A1){
			state = wait;
		}
		break;
		case pressA1:
		
		if(A0){
			state = reset;
		}
		else if(A1){

			state = holdA1;
		}
		else if(!A1){
			state = wait;
		}
		break;
		case reset:
		state = (!A0 && !A1) ? wait : reset;
		break;
		default:
		break;
	}
	switch(state){
		case init:
		break;
		case wait:
		cnt=0;
		break;
		case pressA0:
		if(tmp < 9){
			tmp++;
			LCD_switch(tmp);
		}
		break;
		case holdA0:
		cnt++;
		if(tmp < 9 && cnt >=10)
		{
			tmp++;
			cnt=0;

			LCD_switch(tmp);
			//LCD_DisplayString(1,tmp+30);
		}
		break;
		case holdA1:
		cnt++;
		if(tmp > 0 && cnt >=10)
		{
			tmp--;
			cnt=0;

			LCD_switch(tmp);
			//LCD_DisplayString(1,tmp+30);
		}
		break;
		case pressA1:
		if(tmp > 0){
			tmp--;

			LCD_switch(tmp);
			//LCD_DisplayString(1,tmp+30);
		}
		break;
		case reset:
		tmp = 0x00;
		cnt=0;
		break;
		default:
		break;
	}
}


