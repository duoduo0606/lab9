/*	Author: hwu901
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include<avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
typedef enum States{start, init, play, minus, add}States;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
static unsigned char b =0;
int a[8]={261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};

void set_PWM(double frequency){
  static double current_frequency;
  if(frequency != current_frequency){
    if(!frequency){ TCCR3B &= 0x08;}
    else{ TCCR3B |= 0x03;}
    if(frequency < 0.954){OCR3A = 0xFFFF;}
    else if(frequency > 31250){OCR3A= 0x0000;}
    else {OCR3A = (short)(8000000 / (128*frequency))-1;}
    TCNT3 = 0;
    current_frequency = frequency;
  }
}

void PWM_on(){
  TCCR3A = (1 << COM3A0);
  TCCR3B = (1 << WGM32) | (1 << CS31) |(1 << CS30);
  set_PWM(0);
}

void PWM_off(){
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

void TimerOn(){
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}

void TimerOff(){
  TCCR1B = 0x00;
}

void TimerISR(){
  TimerFlag =1;
}

ISR(TIMER1_COMPA_vect){
  _avr_timer_cntcurr--;
  if(_avr_timer_cntcurr==0){
    TimerISR();
    _avr_timer_cntcurr = _avr_timer_M;
  }
}

void TimerSet(unsigned long M){
  _avr_timer_M = M;
  _avr_timer_cntcurr = _avr_timer_M;
}
  

  int exampleTick(int state){
  unsigned char A0 = ((~PINA) & 0x01);
  unsigned char A1 = ((~PINA) & 0x02);
  unsigned char A2 = ((~PINA) & 0x04);
    switch (state){
    case start:
      state = init;
      break;
    case init:
      if(A0){
	state = play;}
      break;
    case add:
      if(A0){
	state=init;}
      else{
	state = play;}
      break;
    case minus:
      if(A0){
	state=init;}
      else{
	state = play;}
      break;
    case play:
      if(A0){
       state = init;}
     else if(A1 && !A2 && !A0){
	state=add;}
     else if(A2 && !A1 && !A0){
	state=minus;}
  
	break;
    default:
      state = start;
      break;
    }
    switch(state){
    case start:
      break;
    case init:
      set_PWM(0);
      break;
    case play:
      set_PWM(a[b]);
    
        break;
    case add:
      if(b < 7){
	b++;}
      else if(b == 7){
	b = 7;}
      break;
    case minus:
      if(b >0){
	b--;}
      else if(b == 0){
	b = 0;}
      break;
    }
  
    return state;
  }
      
    
int main(void) {
   DDRA = 0x00;PORTA=0xFF;
   DDRB = 0xFF;PORTB=0x00;
   TimerSet(100);
   TimerOn();
   PWM_on();
   States state = start;
   while (1) {
     state = exampleTick(state);
     while(!TimerFlag);
     TimerFlag = 0;
    }
    return 1;
}
