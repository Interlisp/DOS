/* BESSEL.C: This program illustrates Bessel functions including:
 *      j0          j1          jn          y0          y1          yn
 */

#include <math.h>
#include <stdio.h>

void main()
{
    double x = 2.387;
    int n = 3, c;

    printf( "Bessel functions for x = %f:\n", x );
    printf( "  Kind\t\tOrder\tFunction\tResult\n\n" );
    printf( "  First\t\t0\tj0( x )\t\t%f\n", j0( x ) );
    printf( "  First\t\t1\tj1( x )\t\t%f\n", j1( x ) );
    for( c = 2; c < 5; c++ )
        printf( "  First\t\t%d\tjn( n, x )\t%f\n", c, jn( c, x ) );

    printf( "  Second\t0\ty0( x )\t\t%f\n", y0( x ) );
    printf( "  Second\t1\ty1( x )\t\t%f\n", y1( x ) );
    for( c = 2; c < 5; c++ )
        printf( "  Second\t%d\tyn( n, x )\t%f\n", c, yn( c, x ) );
}
