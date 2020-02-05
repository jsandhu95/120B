/*  Author: jsand021 - Jeevan Sandhu
 *  Partner(s) Name: jweis012 - Jason Weiser
 *  Lab Section: 23
 *  Assignment: Lab 6  Exercise 2
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


enum states {init, zero, one, two, holdPress, holdRelease} state;
void tick(){
//    unsigned char A0 = PINA & 0x01;
	
	switch(state){
		case init:
			state = zero;
			break;
			
		case zero:
            if(~PINA & 0x01) {state = holdPress;}
            else {state = one;}
			break;
			
		case one:
            if(~PINA & 0x01){state = holdPress;}
			else {state = two;}
			break;
			
		case two:
            if(~PINA & 0x01) {state = holdPress;}
			else {state = zero;}
			break;

		case holdPress:
            if(~PINA & 0x00) {state = holdRelease;}
			break;

		case holdRelease:
            if(~PINA & 0x01) {state = zero;}
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

		case holdPress:
			break;

		case holdRelease:
			break;

		default:
			break;
	}//Actions
}


int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

	
	TimerSet(300);
	TimerOn();
	state = init;
    while(1)
    {
        tick();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}
