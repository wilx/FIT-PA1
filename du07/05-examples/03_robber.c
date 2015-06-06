#include <stdio.h>
#include <stdlib.h>


/* "Vylepsena" verze. Pozna, ze nema cenu se do prirazovani poustet:
 *    pokud soucet neni delitelny 3 
 *  nebo 
 *    pokud cena nedrazsiho predmetu je vetsi nez 1/3 souctu
 *
 * Dale, protoze nezalezi na ocislovani loupezniku, prvni predmet je 
 *  vzdy prirazen loupeznikovi cislo 1 (ostatni predmety se jiz zkousi 
 *  hrubou silou). Timto jednoduchym trikem se vypocet zkrati az na 1/3.
 *
 * Dale, program se snazi predane predmety prirazovat nejprve rovnomerne, 
 *  (prvni prvemu, druhy druhemu, treti tretimu, ctvrty opet prvemu, ...)
 *  a teprve pokud to nevyjde, zkusit jine prirazeni. Aby tento postup mel 
 *  vetsi sanci na uspech, predmety si funkce serazdi sestupne podle ceny.
 */ 

long long int g_Calls = 0;  /* pocitadlo volani, pro demonstrace */
/*---------------------------------------------------------------------------*/
int tryDivide ( int * values, int valuesNr, int x1, int x2, int x3 )
 {
   g_Calls ++;
   if ( ! valuesNr )
    return ( x1 == x2 && x2 == x3 );

   return ( tryDivide ( values + 1, valuesNr - 1, x2, x3, x1 + *values ) ||
            tryDivide ( values + 1, valuesNr - 1, x1, x3, x2 + *values ) ||
            tryDivide ( values + 1, valuesNr - 1, x1, x2, x3 + *values ) );
 }
/*---------------------------------------------------------------------------*/
int  intSortProc ( const int * a, const int * b )
 {
   return ( *b - *a ); /* sestupne */
 }
/*---------------------------------------------------------------------------*/
int canDivide3 ( int * values, int valuesNr )
 {
   int i, sum, max;
   
   if ( valuesNr == 0 ) return ( 1 );
   sum = values[0];
   max = values[0];
   for ( i = 1; i < valuesNr; i ++ ) 
    {
      sum += values[i];
      if ( values[i] > max ) max = values[i];
    }
   if ( sum % 3 != 0 || max > sum / 3 ) return ( 0 ); 

   qsort ( values, valuesNr, sizeof ( *values ), 
           (int(*)(const void *, const void *))intSortProc );
   return tryDivide ( values+1, valuesNr-1, 0, 0, *values );
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
