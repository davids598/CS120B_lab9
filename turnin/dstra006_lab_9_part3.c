/*	Author: David Strathman
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #9 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 * Link to Vid: https://youtu.be/DyINRZ_2DoU
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "../header/timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
enum SM_States { SM_LOOP, SM_HOLD, SM_PLAY } SM_State;
double freq[] = {523.25, 493.88, 523.25, 392.00, 349.23, 523.25, 493.88, 523.25, 392.00, 349.23}; // C B C G F
unsigned time[] = {3,   3,  6,  6, 6, 3,   3,  6,  6, 6};
unsigned char currA = 0;
unsigned char holdA = 0;
signed char i = 0x00;
unsigned char counter = 0x00;

void set_PWM(double frequency) {
  static double current_frequency;
  if (frequency != current_frequency) {
    if (!frequency) { TCCR3B &= 0x08; }
    else { TCCR3B |= 0x03; }

    if (frequency < 0.954) { OCR3A = 0xFFFF; }

    else if (frequency > 31250) { OCR3A = 0x0000; }

    else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

    TCNT3 = 0;
    current_frequency = frequency;
  }
}

void PWM_on() {
  TCCR3A = (1 << COM3A0);

  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

  set_PWM(0);
}

void PWM_off() {
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

void Tick() {
  switch (SM_State) {
    case SM_LOOP:
      currA = ~PINA;
      if (currA == 0x01) {
        SM_State = SM_PLAY;
      }
      else {
        SM_State = SM_LOOP;
      }
      break;

    case SM_HOLD:
      holdA = ~PINA;
      if (holdA == 0x00) {
        SM_State = SM_LOOP;
        i = 0;
        counter = 0;
      } else {
        SM_State = SM_HOLD;
      }
      break;

    case SM_PLAY:
      if (i >= 10) {
        SM_State = SM_HOLD;
      }
  }

  switch (SM_State) {
    case SM_LOOP:
      break;

    case SM_HOLD:
      set_PWM(0);
      break;

    case SM_PLAY:
      set_PWM(freq[i]);
      PORTB = counter;
      if (counter < time[i]) {
        counter++;
      } else {
        ++i;
        counter = 0;
      }
      break;
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; //A is input
    DDRB = 0xFF; PORTB = 0x00; //Set both to outputs
    /* Insert your solution below */
    TimerSet(100);
    TimerOn();
    PWM_on();
    SM_State = SM_LOOP;
    while (1) {
      Tick();
      while (!TimerFlag);
      TimerFlag = 0;
    }
    return 1;
}
