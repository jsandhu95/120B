/*  Author: jsand021 - Jeevan Sandhu
 *  Partner(s) Name: jweis012 - Jason Weiser
 *  Lab Section: 23
 *  Assignment: Lab 6  Exercise 1
 *  Exercise Description: [optional - include for your own benefit]
 *
 *  I acknowledge all content contained herein, excluding template or example
 *  code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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


enum states {init, zero, one, two} state;
void tick(){
	
	switch(state){
		case init:
			state = zero;
			break;
			
		case zero:
            else {state = one;}
			break;
			
		case one:
			else {state = two;}
			break;
			
		case two:
			else {state = zero;}
			break;

		default:
			break;
	}//Transitions
	
	switch(state){
		case init:
            PORTB = 0x00;
			break;
			
		case zero:
			PORTB = 0x01;
			break;
			
		case one:
			PORTB = 0x02;
			break;
			
		case two:
			PORTB = 0x04;
			break;

		default:
			break;
	}//Actions
}


int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;

	
	TimerSet(1000);
	TimerOn();
	state = init;
    while(1)
    {
        tick();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}
