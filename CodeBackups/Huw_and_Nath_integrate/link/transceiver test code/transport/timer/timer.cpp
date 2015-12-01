/*
 * HelloWorld.c
 *
 *  Created on: 11 Dec 2013
 *      Author: dm6g13
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned long milliseconds;

unsigned long millis()
{
   unsigned long m;
   uint8_t oldSREG = SREG;

   // disable interrupts while we read timer0_millis or we might get an
   // inconsistent value (e.g. in the middle of a write to timer0_millis)
   cli();
   m = milliseconds;
   SREG = oldSREG;
   return m;
}

void init_timer()
{
    TCCR1B = _BV(WGM12) //CTC mode

    | _BV(CS11);   /* 8 prescaler */

    OCR1A = 1499; //Want to count up to 1499 to give 1ms period

    TIMSK1 |= _BV(OCIE1A); //Set up overflow as interrupt

    sei();
}

ISR(TIMER1_COMPA_vect)
{
    cli();
    milliseconds++;
    sei();
}
