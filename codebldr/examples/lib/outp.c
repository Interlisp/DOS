/* OUTP.C: This program uses inp and outp to make sound of variable tone
 * and duration.
 */

#include <conio.h>
#include <stdio.h>
#include <time.h>

void Beep( unsigned duration, unsigned frequency ); /* Prototypes */
void Sleep( clock_t wait );

void main ()
{
    Beep( 698, 700 );
    Beep( 523, 500 );
}

/* Sounds the speaker for a time specified in microseconds by duration
 * at a pitch specified in hertz by frequency.
 */
void Beep( unsigned frequency, unsigned duration )
{
    int control;

    /* If frequency is 0, Beep doesn't try to make a sound. */
    if( frequency )
    {
        /* 75 is about the shortest reliable duration of a sound. */
        if( duration < 75 )
            duration = 75;

        /* Prepare timer by sending 10111100 to port 43. */
        outp( 0x43, 0xb6 );

        /* Divide input frequency by timer ticks per second and
         * write (byte by byte) to timer.
         */
        frequency = (unsigned)(1193180L / frequency);
        outp( 0x42, (char)frequency );
        outp( 0x42, (char)(frequency >> 8) );

        /* Save speaker control byte. */
        control = inp( 0x61 );

        /* Turn on the speaker (with bits 0 and 1). */
        outp( 0x61, control | 0x3 );
    }

    Sleep( (clock_t)duration );

    /* Turn speaker back on if necessary. */
    if( frequency )
        outp( 0x61, control );
}

/* Pauses for a specified number of microseconds. */
void Sleep( clock_t wait )
{
    clock_t goal;

    goal = wait + clock();
    while( goal > clock() )
        ;
}
