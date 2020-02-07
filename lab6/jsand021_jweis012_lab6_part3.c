/*	Author: jsand021 - Jeevan Sandhu
 *  Partner(s) Name: jweis012 - Jason Weiser
 *	Lab Section: 23
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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


typedef enum States {init, wait, pressA0, pressA1, reset} State;
typedef enum bool {false, true} bool;

int press(int, bool*);

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;

    State state = init;
    bool flag = false;
    unsigned char cnt = 0;


    TimerSet(100);
    TimerOn();
    while(1)
    {
        state = press(state, &flag); 
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}

int press(int state, bool *flag){
   unsigned char A0 = PINA & 0x01;
   unsigned char A1 = (PINA & 0x02) >> 1;

   switch(state){
      case init:
         PORTC = 0x07;
         *flag = false;
         state = wait;
         break;
      case wait:
         cnt = 0;
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
      case pressA0:
         if(A1){
            state = reset;
         }
         else if(A0){
            state = pressA0;
         }
         else if(!A0){
            state = wait;
         }
         break;
      case pressA1:
         if(A0){
            state = reset;
         }
         else if(A1){
            state = pressA1;
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
         *flag = false;
         break;
      case pressA0:
         if(PORTC < 9 && (*flag) == false){
            PORTC++;
            *flag = true;
         }
         break;
      case pressA1:
         if(PORTC > 0 && (*flag) == false){
            PORTC--;
            *flag = true;
         }
         break;
      case reset:
         PORTC = 0x00;
         break;
      default:
         break;
   }
   return state;
}
