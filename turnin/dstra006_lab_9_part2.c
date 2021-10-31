/*	Author: David Strathman
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #9 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 * Link to Vid: https://youtu.be/zZFfmf22BqM
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
enum SM_States { SM_LOOP, SM_UP, SM_DOWN, SM_HOLD, SM_TOGGLE } SM_State;
double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char currA = 0;
unsigned char holdA = 0;
signed char i = 0x00;
unsigned char state = 0x01;

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
        SM_State = SM_UP;
      }
      else if (currA == 0x02) {
        SM_State = SM_DOWN;
      }
      else if (currA == 0x04) {
        SM_State = SM_TOGGLE;
      } else {
        SM_State = SM_LOOP;
      }
      break;

    case SM_UP:
      SM_State = SM_HOLD;
      break;
    case SM_DOWN:
      SM_State = SM_HOLD;
      break;

    case SM_HOLD:
    holdA = ~PINA;
      if (holdA == 0x00) {
        SM_State = SM_LOOP;
      } else {
        SM_State = SM_HOLD;
      }
      break;

    case SM_TOGGLE:
      SM_State = SM_HOLD;
  }

  switch (SM_State) {
    case SM_LOOP:
      if (state == 0x01) {
        set_PWM(freq[i]);
      } else {
        set_PWM(0);
      }
      break;

    case SM_UP:
      if (i <= 6) {
        i = i + 1;
      }
      break;

    case SM_DOWN:
      if (i >= 1) {
        i = i - 1;
      }
      break;

    case SM_HOLD:
      if (state == 1) {
        set_PWM(freq[i]);
      } else {
        set_PWM(0);
      }
      break;

    case SM_TOGGLE:
      if (state == 1) {
        state = 0;
      } else {
        state = 1;
      }
      break;
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; //A is input
    DDRB = 0xFF; PORTB = 0x00; //Set both to outputs
    /* Insert your solution below */
    PWM_on();
    set_PWM(freq[i]);
    SM_State = SM_LOOP;
    while (1) {
      Tick();
    }
    return 1;
}
