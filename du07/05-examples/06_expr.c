#include <ctype.h>
#include <stdio.h>

/* Vyhodnoceni vyrazu - zohlednuje priority, umi zpracovavat zavorky
 */

#define LEX_NULL    0
#define LEX_ERR     1
#define LEX_INT     2
#define LEX_ADD     3
#define LEX_SUB     4
#define LEX_MUL     5
#define LEX_DIV     6
#define LEX_LPA     7
#define LEX_RPA     8

/* Tokenizer rozdeli vstupni retezec na lex. elementy (tokeny) - cisla, operatory, zavorky.
 * Kazde zavolani tokenizeru da "dalsi" token ze vstupu. Parametr str je typu IN/OUT. Funkce
 * nextToken jej upravi tak, aby pri dalsim zavolani cetla od pozice, kde posledne skoncila.
 *
 */
/*-----------------------------------------------------------------------------------------------*/
int  nextToken ( char ** str, double * val )
 {
   char c;

   while ( isspace ( **str )) (*str)++;

   if ( ! **str ) return ( LEX_NULL );
   c = *(*str)++;
   if ( c == '+' ) return ( LEX_ADD );
   if ( c == '-' ) return ( LEX_SUB );
   if ( c == '*' ) return ( LEX_MUL );
   if ( c == '/' ) return ( LEX_DIV );
   if ( c == '(' ) return ( LEX_LPA );
   if ( c == ')' ) return ( LEX_RPA );

   if ( ! isdigit ( c ) ) return ( LEX_ERR );

  *val = 0;
   while ( isdigit ( c ) )
    {
      *val = *val * 10 + c - '0';
      c = *(*str)++;
    }
   (*str)--;
   return ( LEX_INT );
 }
/*-----------------------------------------------------------------------------------------------*/
int  expr ( char ** str, double * res, int whenToReturn );
/*-----------------------------------------------------------------------------------------------*/
/* faktor je bud primo cislo (int) nebo slozitejsi vyraz v zavorkach.
 * Vyraz v zavorkach vyhodnotime rekurzivne - volanim funkce expr (jen musime spravne nastavit
 * omezovac, kdy se ma funcke vratit - na pravou zavorku).
 *
 * parameter str  je typu char **, aby zmeny v pozici ve vstupnim retezci byly zachovane i po
 * navratu z rekurzivnich volani.
 */
int  factor ( char ** str, double * val )
 {
   int tok;

   tok = nextToken ( str, val );
   if ( tok == LEX_INT ) return ( 1 );
   if ( tok != LEX_LPA ) return ( 0 );
   return ( expr ( str, val, LEX_RPA ) ); /* neprima rekurze */
 }
/*-----------------------------------------------------------------------------------------------*/
int  expr ( char ** str, double * res, int whenToReturn )
 {
   double x, dummy, mul;
   int    token;

   /* Obecne ma vyraz formu:
    * cislo op cislo op cislo .... op cislo
    * prvni cislo je nacteno mimoi cyklus
    * dvojice op cislo se muze opakovat, proto je ctena v cyklu
    *
    * Do vysledku (res) ukladame pouze ty termy, ktere jiz nebudou nasobeny/deleny
    * zadnym dalsim cislem ze vstupu. Rozpracovane termy (ty, ktere mohou dale byt n
    * asobeny/deleny) ukladame do pomocne promenne mul. V okamziku dalsiho scitani/
    * odcitani/konce vstupu pridame obsah mul k vysledku.
    */
    
   *res = 0;
   if ( ! factor ( str, &mul ) ) return ( 0 );

   while ( 1 )
    {
      token = nextToken ( str, &dummy );

      if ( token == whenToReturn )
       {
        *res += mul;
         return ( 1 );
       }

      if ( ! factor ( str, &x ) ) return ( 0 );

      switch ( token )
       {
         case LEX_ADD:
         *res += mul;
          mul = x;
          break;
         case LEX_SUB:
         *res += mul; /* zde += mul, operator - se vztahuje k dalsimu termu */
          mul = -x;   /* proto jej uplatnime az zde */
          break;
         case LEX_MUL:
          mul *= x;
          break;
         case LEX_DIV:
          mul /= x;
          break;
         default:
          return ( 0 );
       }
    }
 }
/*-----------------------------------------------------------------------------------------------*/
int main ( int argc, char * argv [] )
 {
   double val;
   char str [100], * tmp;

   printf ( "Zadejte vyrazy:\n" );
   while ( fgets ( str, sizeof ( str ), stdin ) )
    {
      tmp = str;
      if ( expr ( &tmp, &val, LEX_NULL ) )
        printf ( "= %f\n", val );
       else
        printf ( "Chybny vyraz.\n" );
    }
   if ( ! feof ( stdin ) )
    {
      printf ( "I/O chyba.\n" );
      return ( 1 );
    }
   return ( 0 );
 }
/*-----------------------------------------------------------------------------------------------*/

