#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


enum constants
{
    MAX_DIGITS = 32,

    /* We assume input will not be longer than MAX_DIGITS binary digits.
       Thosecan be combined using at most MAX_DIGITS - 1 arithmetic operations.
       And we need the terminating '\0'.  */
    BUF_LEN = MAX_DIGITS + (MAX_DIGITS - 1) + 1,

    /* What part of the whole expression do we want to generate? */
    PART_NUMBER = 0,
    PART_OP
};


enum token_tag
{
    LEX_ERR = -1,
    LEX_NULL,
    LEX_ADD,
    LEX_SUB,
    LEX_MUL,
    LEX_DIV,
    LEX_NUMBER
};

typedef enum token_tag token_type;


int target;
int digits;


void
input_error (void)
{
    printf ("Nespravny vstup.\n");
    exit (EXIT_FAILURE);
}


/* *, 42 | +, 43 | ,, 44 | -, 45 | ., 46 | /, 47 | 0, 48 | 1, 49 */
static token_type const chtbl[8] =
{
    /* *, 42 */
    LEX_MUL, 
    /* +, 43 */
    LEX_ADD,
    /* ,, 44 */
    LEX_ERR,
    /* -, 45 */
    LEX_SUB,
    /* ., 46 */
    LEX_ERR,
    /* /, 47 */
    LEX_DIV,
    /* 0, 48 */
    LEX_NUMBER,
    /* 1, 49 */
    LEX_NUMBER
};


int
is_bin_digit (char ch)
{
    return '0' <= ch && ch <= '1';
}


token_type
nextToken (char const * * str, double * val)
{
    char c;
    token_type tok;

    c = **str;
    if (! c)
        return LEX_NULL;
    if (! ('*' <= c && c <= '1'))
        return LEX_ERR;

    ++(*str);

    tok = chtbl[c - '*'];
    if (tok == LEX_NUMBER)
    {
        *val = 0;
        for (; is_bin_digit (c); c = *(*str)++)
        {
            *val = *val * 2 + c - '0';
        }

        --(*str);
    }

    return tok;
}


int
expr (char const * str, double * res)
{
    double x, dummy, mul;
    int token;

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
    if (nextToken (&str, &mul) != LEX_NUMBER)
        return 0;

    while (1)
    {
        token = nextToken (&str, &dummy);

        if (token == LEX_NULL)
        {
            *res += mul;
            return 1;
        }

        if (nextToken (&str, &x) != LEX_NUMBER)
            return 0;

        switch (token)
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
            return 0;
        }
    }
}


int
gen_solutions (char buf[BUF_LEN], int pos, int digits_count, char prev_type)
{
    int count = 0;

    if (digits_count == digits)
    {
        double val = 0;
        int ret;

        buf[pos] = '\0';
        ret = expr (buf, &val);
        if (! ret)
            input_error ();

        if (fabs (val - target) <= 0.0001)
        {
            printf ("%s bin = %d dec\n", buf, target);
            return 1;
        }
        else
            return 0;
    }

    buf[pos] = '0';
    count += gen_solutions (buf, pos + 1, digits_count + 1, PART_NUMBER);
    
    buf[pos] = '1';
    count += gen_solutions (buf, pos + 1, digits_count + 1, PART_NUMBER);

    if (prev_type == PART_NUMBER)
    {
        buf[pos] = '+';
        count += gen_solutions (buf, pos + 1, digits_count, PART_OP);
        buf[pos] = '-';
        count += gen_solutions (buf, pos + 1, digits_count, PART_OP);
        buf[pos] = '*';
        count += gen_solutions (buf, pos + 1, digits_count, PART_OP);
        buf[pos] = '/';
        count += gen_solutions (buf, pos + 1, digits_count, PART_OP);
    }

    return count;
}


int
main (void)
{
    int ret;
    char buf[BUF_LEN];

    printf ("Zadejte pozadovany vysledek:\n");
    ret = scanf ("%d", &target);
    if (ret != 1)
        input_error ();

    printf ("Zadejte pocet cifer:\n");
    ret = scanf ("%d", &digits);
    if (ret != 1 || digits <= 0)
        input_error ();

    ret = gen_solutions (buf, 0, 0, PART_OP);
    printf ("Celkem reseni: %d\n", ret);

    return EXIT_SUCCESS;
}
