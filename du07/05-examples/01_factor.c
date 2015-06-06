#include <stdio.h>
#include <math.h>
/* Faktorizace cisla na prvocinitele. Pokud se podari nalezt delitele zadaneho cisla,
 * jsou tito delitele dale rekurzivne faktorizovani. Naopak, pokud funkce zjisti, ze
 * delitel neexistuje, vypise faktorizovane cislo (je to prvocielny faktor).
 *
 * Promenna first slozi k potlaceni hvezdicky pred prvnim faktorem
 */
void factorize ( int x, int first )
 {
   int i, max = (int) sqrt ( x );
   
   for ( i = max; i >= 2; i -- )
    if ( x % i == 0 )
     {
       factorize ( i, first );
       factorize ( x / i, 0 );
       return;
     }

   if ( ! first ) 
    printf ( " * " );
   printf ( "%d", x );  
 }

/* Jina varianta reseni, nepotrebuje promennou first.
 */
void factorize2 ( int x )
 {
   int i, max = (int) sqrt ( x );
   
   for ( i = max; i >= 2; i -- )
    if ( x % i == 0 )
     {
       factorize2 ( i );
       printf ( " * " );
       factorize2 ( x / i );
       return;
     }
   printf ( "%d", x );  
 }


int main ( int argc, char * argv [] )
 {
   int n;

   /* Parametry (faktorizovane cislo) cteme z prikazove radky.
    */
   if ( argc != 2 || sscanf (argv[1],  "%d", &n ) != 1 || n <= 0 ) 
    {
      printf ( "Nespravny parametr.\n" );
      return ( 1 );
    }
   printf ( "%d = ", n );    
   factorize ( n, 1 );
   printf ( "\n" ); 
   return ( 0 ); 
 }
 
