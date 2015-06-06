#include <stdio.h>

#define ARRAY_LEN 12

int g_Cnt = 0;

/* Rekurzivni reseni - primerene efektivni. Rekurze zde nahrazuje iteraci.
 */
int maximum1 ( int * array, int len )
 {
   int max;
   g_Cnt ++;
   if ( len == 1 ) return ( *array );
   max = maximum1 ( array + 1, len - 1 );
   return ( max > *array ? max : *array );
 }

/* Rekurzivni reseni - neefektivni. Pokud prvni prvek neni vetsi nez zbytek prvku v poli,
 * prohledava se zbytek pole 2x. To se deke pro kazdou pozici v poli. Slozitost je tedy
 * az exponencialni (pro vzestupne setrizene pole).
 */
 
int maximum2 ( int * array, int len )
 {
   g_Cnt ++;
   if ( len == 1 ) return ( *array );
   return ( *array > maximum2 ( array + 1, len - 1 ) ? *array : maximum2 ( array + 1, len - 1 ) );
 }

int main ( int argc, char * argv [] )
 {
/*
   int array[ARRAY_LEN] = { 3,7,4,8,12,6,2,10,45,17,29,15 };
   int array[ARRAY_LEN] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
*/
   int array[ARRAY_LEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
   int max;

   max = maximum2 ( array, ARRAY_LEN );

   printf ( "Maximum je: %d\n", max );
   printf ( "Pocet volani: %d\n", g_Cnt );
   return ( 0 );
 }
