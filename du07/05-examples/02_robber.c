#include <stdio.h>

/* Neoptimalizovana verze. Pouze se hrubou silou snazi projit vsechny moznosti
 * prirazeni predmetu loupeznikum. Pokud se ji podari najit shodu, hlasi uspech.
 *
 */

long long int g_Calls = 0; /* pocet volani funkce v rekurzi, pro ilustraci */

/*---------------------------------------------------------------------------*/
int tryDivide ( int * values, int valuesNr, int x1, int x2, int x3 )
 {
   g_Calls ++;
   
   if ( ! valuesNr )
    return ( x1 == x2 && x2 == x3 );

   return ( tryDivide ( values + 1, valuesNr - 1, x1 + *values, x2, x3 ) ||
            tryDivide ( values + 1, valuesNr - 1, x1, x2 + *values, x3 ) ||
            tryDivide ( values + 1, valuesNr - 1, x1, x2, x3 + *values ) );
 }
/*---------------------------------------------------------------------------*/
int canDivide3 ( int * values, int valuesNr )
 {
   return tryDivide ( values, valuesNr, 0, 0, 0 );
 }
/*---------------------------------------------------------------------------*/
int main ( int argc, char * argv [] )
 {
/*
   int items[] = { 10, 12, 18, 6, 8 };
   int items[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
   int items[] = { 10, 11, 12 };
   int items[] = { 10, 10, 9, 2, 2 };
   int items[] = { 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
                           1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
                           10, 20 };
   int items[] = { 1, 1, 1, 2, 2, 2, 4, 4, 4, 8, 8, 8,
                           16, 16, 16, 32 ,32, 32, 64, 64, 64 };
*/
   int items[] = { 1, 1, 1, 2, 2, 2, 4, 4, 4, 8, 8, 8,
                           16, 16, 16, 32 ,32, 32, 64, 64, 64 };

   
   printf ( "Predmety %slze rozdelit spravedlive.\n", 
             canDivide3 ( items, sizeof ( items ) / sizeof ( items[0] ) ) ? "" : "ne" );

   printf ( "Celkem provedeno %lld volani.\n", g_Calls );
   return ( 0 );
 }
