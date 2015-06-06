#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif /* __PROGTEST__ */


static
int
max_int (int a, int b)
{
    return a > b ? a : b;
}


static
void
lcs_b (size_t m, size_t n, char const * A, char const * B, int * LL)
{
    int * K0 = NULL;
    int * K1 = NULL;
    size_t i;
    size_t j;
    size_t const K_len = n + 1;
    size_t const K_size = K_len * sizeof (int);

    /* step 1 */

    K0 = (int *)malloc (n, K_size);
    if (K0 == NULL)
        goto error;

    K1 = (int *)calloc (n, K_size);
    if (K1 == NULL)
        goto error;

    /* step 2 */

    for (i = 1; i <= n; ++i)
    {
        /* step 3 */

        memcpy (K0, K1, K_size);

        /* step 4 */

        for (j = 1; j <= n; ++j)
        {
            if (A[i] == B[j])
                K1[j] = K0[j - 1] + 1;
            else
            {
                /* The following chunk is necessary if we are searching
                   for subsequence:

                   K1[j] = max_int (K1[j - 1], K0[j]);

                   But we want only substrings.
                */

                K1[j] = K0[j];
            }
        }
    }

    /* step 5 */

    memcpy (LL, K1, K_size);

    /* Clean up. */

    free (K0);
    free (K1);

    return;

error:
    free (K0);
    free (K1);
    
    exit (EXIT_FAILURE);
}





char * *
LCS (char * s1, char * s2) 
{
    size_t s1_len = strlen (s1);
    size_t s2_len = strlen (s2);
    
    char * * result = NULL;

    return result;
}


#ifndef __PROGTEST__
int
main (void)
{

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
