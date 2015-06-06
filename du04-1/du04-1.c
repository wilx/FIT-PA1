#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


typedef double item_type;
#define ITEM_SC_FMT "%f"


struct matrix_tag
{
    unsigned x;
    unsigned y;
    item_type * m;
};


typedef struct matrix_tag matrix_type;


static
void
malloc_error ()
{
    printf ("Chyba alokace.\n");
    exit (EXIT_FAILURE);
}


void
matrix_create (matrix_type * m, unsigned x, unsigned y)
{
    assert (x > 0);
    assert (y > 0);

    m->x = x;
    m->y = y;
    m->m = (double *)malloc (sizeof (item_type) * x * y);
    if (! m->m)
        malloc_error ();

    return m;
}


void
matrix_copy (matrix_type * m, matrix_type const * src)
{
    m->x = src->x;
    m->y = src->y;
    m->m = (double *)malloc (sizeof (item_type) * m->x * m->y);
    memcpy (m->m, src->m, sizeof (item_type) * m->x * m->y);
}


item_type *
matrix_get_row (matrix_type * m, unsigned r)
{
    return &m->m[r * m->x];
}


item_type *
matrix_pget (matrix_type * m, unsigned x, unsigned y)
{
    return &m->m[x + (y * m->x)];
}


item_type
matrix_get (matrix_type * m, unsigned x, unsigned y)
{
    *matrix_pget (m, x, y);
}


void
bad_input_error ()
{
    printf ("Nespravny vstup.\n");
    exit (EXIT_FAILURE);
}


int
main (void)
{
    int ret;
    int mn;
    int x, y;
    int varcount;
    matrix_type m;
    item_type * pnum;
    int i, j;

    printf ("Zadejte pocet promennych:\n");
    ret = scanf ("%d", &varcount);
    if (ret != 1 || varcount <= 1)
        bad_input_error ();

    y = varcount;
    x = varcount + 1;

    matrix_create (&m, x, y);
    
    printf ("Zadejte %d prvku matice:\n", x * y);
    for (i = 0; i != x * y; ++i)
    {
        ret = scanf ("%f", &m->m[i]);
        if (ret != 1)
            bad_input_error ();
    }
    

    /* Loop over columns. */
    for (k = 0; k != varcount; ++k)
    {
        item_type const * pivot_row = matrix_get_row (&m, k);
                        
        /* Loop over rows. */
        for (j = 0; j != varcount; ++j)
        {
            double pivot = pivot_row[k];
            
            /* Loop over columns of each row. */
            for (i = j; i != varcount; ++i)
            {
                
            }
        }
    }
}
