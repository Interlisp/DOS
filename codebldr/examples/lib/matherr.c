/* MATHERR.C: To use matherr, you must turn off the Extended Dictionary
 * flag within the Microsoft Programmer's WorkBench environment, or use
 * the /NOE linker option outside the environment. For example:
 *      CL matherr.c /link /NOE
 */

#include <math.h>
#include <string.h>
#include <stdio.h>

void main()
{
   /* Do several math operations that cause errors. The matherr
    * routine handles DOMAIN errors, but lets the system handle
    * other errors normally.
    */
   printf( "log( -2.0 ) = %e\n", log( -2.0 ) );
   printf( "log10( -5.0 ) = %e\n", log10( -5.0 ) );
   printf( "log( 0.0 ) = %e\n", log( 0.0 ) );
}

/* Handle several math errors caused by passing a negative argument
 * to log or log10 (DOMAIN errors). When this happens, matherr returns
 * the natural or base-10 logarithm of the absolute value of the
 * argument and suppresses the usual error message.
 */
int matherr( struct exception *except )
{
    /* Handle DOMAIN errors for log or log10. */
    if( except->type == DOMAIN )
    {
        if( strcmp( except->name, "log" ) == 0 )
        {
            except->retval = log( -(except->arg1) );
            printf( "Special: using absolute value: %s: DOMAIN error\n",
                    except->name );
            return 1;
        }
        else if( strcmp( except->name, "log10" ) == 0 )
        {
            except->retval = log10( -(except->arg1) );
            printf( "Special: using absolute value: %s: DOMAIN error\n",
                    except->name );
            return 1;
        }
    }
    else
    {
        printf( "Normal: " );
        return 0;    /* Else use the default actions */
    }
}
