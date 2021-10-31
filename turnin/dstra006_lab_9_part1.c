/*	Author: David Strathman
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #9 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 * Link to Vid:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
enum SM_States { SM_LOOP } SM_State;
unsigned char currA = 0;

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
      SM_State = SM_LOOP;
  }

  switch (SM_State) {
    case SM_LOOP:
      currA = ~PINA;
      if (currA == 0x01) {
        set_PWM(261.63);
        PORTB = 0x01;
      }
      else if (currA == 0x02) {
        set_PWM(293.66);
        PORTB = 0x02;
      }
      else if (currA == 0x04) {
        set_PWM(329.63);
        PORTB = 0x04;
      } else {
        set_PWM(0);
        PORTB = 0x00;
      }
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; //A is input
    DDRB = 0xFF; PORTB = 0x00; //Set both to outputs
    /* Insert your solution below */
    PWM_on();
    SM_State = SM_LOOP;
    while (1) {
      Tick();
    }
    return 1;
}
