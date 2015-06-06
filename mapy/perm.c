#include <stdio.h>
#include <assert.h>


enum constants
{
    MAX = 20
};


int p[MAX];
int c[MAX];


void
exch (int a, int b)
{
    int tmp = p[a];
    p[a] = p[b];
    p[b] = tmp;
}


void
doit (int N)
{
    int i;

    for (i = 0; i != N; ++i)
    {
        printf ("%d ", p[i]);
    }
    
    printf ("\n");
}


#if 0
void
generate (int N)
{
    int n, t, M;

    assert (N <= MAX);

    for (n = 0; n < N; ++n)
    {
        p[n] = n;
        c[n] = 0;
    }

    doit(N);

    for (n = 0; n < N; )
    {
        if (c[n] < n)
        {
            exch(n % 2 ? 0 : c[n], N-1);
            c[n]++;
            n = 0;
            doit(N);
        }
        else
            c[n++] = 0;
    }
}
#endif


void
generate(int n)
{ 
    /* int c; */

    /* if (N == 1) */
    /* { */
    /*     doit();  */
    /*     return;  */
    /* } */

    /* for (c = 0; c < N; c++) */
    /* { */
    /*     generate (N - 1); */
    /*     exch ((N + 1) % 2 ? 0 : c, N); */
    /* } */

    int i;

    if (n == 1)
        doit (4);
    else
    {
        for (i = 0; i < n; i++)
        {
            generate (n-1);

            if (n % 2 == 1) // if n is odd
                exch (0, n-1);
            else            // if n is even
                exch (i, n-1);
        }
    }
}


int
main (void)
{
    int i;

    for (i = 0; i != MAX; ++i)
        p[i] = i;

    generate (4);

    return 0;
}
