#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#endif /* __PROGTEST__ */


static
int
max_int (int a, int b)
{
    return a > b ? a : b;
}


static
void
single_step (char ch_B, char const * const A, size_t A_len,
    int const * const K0, int * const K1, int * const LL)
{
    size_t i;

    K1[0] = 0;

    for (i = 1; i <= A_len; ++i)
    {
        if (ch_B == A[i - 1])
            K1[i] = K0[i - 1] + 1;
        else
            K1[i] = 0;

        LL[i] = max_int (LL[i], K1[i]);
    }
}


static
int
str_cmp (void const * a, void const * b)
{
    char const * x = *(char const * *)a;
    char const * y = *(char const * *)b;

    return strcmp (x, y);
}


static
int
str_eq (char const * a, char const * b)
{
    return strcmp (a, b) == 0;
}


static
int
is_dna (char const * dna)
{
    for (; *dna != 0; ++dna)
    {
        char ch = *dna;
        if (! (ch == 'A' || ch == 'T' || ch == 'C' || ch == 'G'))
            return 0;
    }

    return 1;
}


static
void
free_result (char * * result)
{
    char * * tmp = result;

    if (result == NULL)
        return;

    while (*tmp != NULL)
    {
        free (*tmp);
        ++tmp;
    }

    free (result);
}


char * *
LCS (char * A, char * B)
{
    char * * result = NULL;

    size_t A_len = strlen (A);
    size_t B_len = strlen (B);
    char * s_tmp;

    int * K0 = NULL;
    int * K1 = NULL;
    size_t K_len;
    size_t K_size;

    int * LL = NULL;
    int LL_max;
    size_t LL_max_count;

    size_t i;
    size_t j;

    /* Check input for correctness. */

    if (! is_dna (A) || ! is_dna (B))
        return NULL;

    /* Swap strings if B is shorter than A. */

    if (B_len < A_len)
    {
        size_t s_len_tmp;

        s_tmp = B;
        s_len_tmp = B_len;

        B = A;
        B_len = A_len;

        A = s_tmp;
        A_len = s_len_tmp;
    }

    /* Prepare working area. */

    K_len = A_len + 1;
    K_size = K_len * sizeof (int);

    K0 = (int *)calloc (1, K_size);
    if (K0 == NULL)
        goto error;

    K1 = (int *)malloc (K_size);
    if (K1 == NULL)
        goto error;

    LL = (int *)calloc (1, K_size);
    if (LL == NULL)
        goto error;

    /* Compute the last row of the longest common substring matrix. */

    for (i = 0; i != B_len; ++i)
    {
        int * K_tmp;

        single_step (B[i], A, A_len, K0, K1, LL);

        K_tmp = K0;
        K0 = K1;
        K1 = K_tmp;
    }

    /* Evaluate the last row. Find the length of the longest
       substring. */

    LL_max = 0;
    LL_max_count = 0;
    for (i = 1; i != K_len; ++i)
    {
        if (LL[i] > LL_max)
        {
            LL_max = LL[i];
            LL_max_count = 1;
        }
        else if (LL[i] == LL_max)
            ++LL_max_count;
    }

    /* Allocate result variable storage. */

    result = (char * *)calloc (LL_max_count + 1, sizeof (char *));
    if (result == NULL)
        goto error;

    result[LL_max_count] = NULL;

    for (i = 0; i != LL_max_count; ++i)
    {
        result[i] = (char *)malloc (LL_max + 1);
        if (result[i] == NULL)
            goto error;

        result[i][LL_max] = '\0';
    }

    /* Collect all the longest substrings into result variable, if
       there are any. */

    if (LL_max != 0)
    {
        for (i = K_len - 1, j = 0; i != 0; --i)
        {
            if (LL[i] == LL_max)
            {
                char const * const ssstart = &A[i - 1] - (LL_max - 1);
                memcpy (result[j], ssstart, LL_max);
                ++j;
            }
        }
        
        /* Sort the results variable. */
        
        qsort (result, LL_max_count, sizeof (char *), str_cmp);
        
        /* Remove duplicates. */

        s_tmp = result[0];
        for (i = 0, j = 0; i != LL_max_count; ++i)
        {
            if (i == j)
                continue;
            
            if (str_eq (result[i], result[j]))
            {
                free (result[i]);
                result[i] = NULL;
            }
            else
            {
                result[j + 1] = result[i];
                ++j;
            }
        }
        
        result[j + 1] = NULL;
    }
    else
    {
        free_result (result);
        result = NULL;
    }

    free (K0);
    free (K1);
    free (LL);

    return result;

error:
    free_result (result);

    free (K0);
    free (K1);
    free (LL);

    exit (EXIT_FAILURE);
}


#ifndef __PROGTEST__

static
size_t
print_result (char * * result)
{
    size_t count = 0;

    if (result == NULL)
    {
        printf ("NULL\n");
        return 0;
    }

    printf ("{\n");
    while (*result != NULL)
    {
        printf ("%s,\n", *result);
        ++result;
        ++count;
    }
    printf ("NULL\n}\n");

    return count;
}


static
void
run_test (char * A, char * B)
{
    char * * res;
    
    res = LCS (A, B);
    print_result (res);
    free_result (res);
}


int
main (void)
{
    run_test ("ATTTATTTA", "GTTTCTTTG");
    run_test ( "ATAT", "TATA" );
    run_test ( "AATCG", "CGGCGA" );
    run_test ( "AAAATCCGCGCGCCACGCT", "ACGCTCAAAATTTTTGCGCG" );
    run_test ( "AAAAAATTTTTTTCCCCCCGGGGGGAAAAAA", "ATCGA" );
    run_test ( "AAAAAAAAA", "TTTTTT" );
    run_test ( "AAA", "AAAA " );

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
