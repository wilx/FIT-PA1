#include <stdio.h>
#include <stdlib.h>


struct point
{
    int x;
    int y;
};

typedef struct point point_type;


/* Reads point with given name from the standard input into point_type
 * variable. */
int
read_point (point_type * p, char name)
{
    int ret;

    printf ("Zadejte souradnice bodu %c:\n", name);
    ret = scanf ("%d %d", &p->x, &p->y);
    if (ret != 2)
    {
        printf ("Nespravny vstup.\n");
        return 0;
    }

    return 1;
}


/** Returns 1 for equal points, 0 otherwise. */ 
int
same_point (point_type p1, point_type p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}


/** Returns square of distance between two points. */
int
point_distsq (point_type p1, point_type p2)
{
    int xdist = p1.x - p2.x;
    int ydist = p1.y - p2.y;
    return xdist * xdist + ydist * ydist;
}


/** Initializes row for linear dependence test of three points. */
void
init_row (int (* r)[3], point_type p)
{
    (*r)[0] = p.x;
    (*r)[1] = p.y;
    (*r)[2] = 1;
}


/** Returns determinant of 3x3 matrix. */
int
determinant (int (* m)[3][3])
{
    return
        + (*m)[0][0] * (*m)[1][1] * (*m)[2][2]
        + (*m)[0][1] * (*m)[1][2] * (*m)[2][0]
        + (*m)[1][0] * (*m)[2][1] * (*m)[0][2]
        - (*m)[0][2] * (*m)[1][1] * (*m)[2][0]
        - (*m)[1][0] * (*m)[0][1] * (*m)[2][2]
        - (*m)[1][2] * (*m)[2][1] * (*m)[0][0];
}


void
print_middle (char p)
{
    printf ("Prostredni je bod %c.\n", p);
}


int
main ()
{
    int ret;
    point_type a, b, c;
    int m[3][3];
    int det;

    /* Read input, three points. */

    ret = read_point (&a, 'A');
    if (! ret)
        return EXIT_FAILURE;

    ret = read_point (&b, 'B');
    if (! ret)
        return EXIT_FAILURE;

    ret = read_point (&c, 'C');
    if (! ret)
        return EXIT_FAILURE;

    /* Test linear dependence. */

    init_row (&m[0], a);
    init_row (&m[1], b);
    init_row (&m[2], c);
    det = determinant (&m);
    if (det == 0)
    {
        printf ("Body lezi na jedne primce.\n");
        
        if (same_point (a, b) || same_point (b, c) || same_point (c, a))
            printf ("Nektere body splyvaji - zadny neni uprostred.\n");
        else
        {
            /* The two most distant points are single out the middle
             * point. */

            int dab = point_distsq (a, b);
            int dbc = point_distsq (b, c);
            int dca = point_distsq (c, a);
            if (dab > dbc)
            {
                if (dab > dca)
                    print_middle ('C');
                else
                    print_middle ('B');
            }
            else
            {
                if (dbc > dca)
                    print_middle ('A');
                else
                    print_middle ('B');
            }
        }
    }
    else
        printf ("Body nelezi na jedne primce.\n");

    return EXIT_SUCCESS;
}
