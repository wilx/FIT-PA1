/*
  Copyright (c) 2010, Vaclav Haisman

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>


#define WN_UNIT (2)


typedef int coord_type;
#define COORD_FMT "%d"


struct point3
{
    coord_type x;
    coord_type y;
    coord_type z;
};

typedef struct point3 point_type;


struct vec3
{
    coord_type x;
    coord_type y;
    coord_type z;
};

typedef struct vec3 vector_type;


static point_type const origin = { 0, 0, 0 };


static
void
init_vector (vector_type * v, point_type const * p1, point_type const * p2)
{
    v->x = p2->x - p1->x;
    v->y = p2->y - p1->y;
    v->z = p2->z - p1->z;
}


static
void
cross_prod (vector_type * w, vector_type const * u, vector_type const * v)
{
    w->x = u->y * v->z - u->z * v->y;
    w->y = u->z * v->x - u->x * v->z;
    w->z = u->x * v->y - u->y * v->x;
}


/* Reads point with given name from the standard input into point_type
 * variable. */
static
int
read_point (point_type * p)
{
    int ret;

    p->z = 0;
    ret = scanf (COORD_FMT " " COORD_FMT, &p->x, &p->y);
    return ret == 2;
}


static
void
bad_input_error (void)
{
    printf ("Nespravny vstup.\n");
    exit (EXIT_FAILURE);
}


static
void
non_convex_error (void)
{
    printf ("Zadany n-uhelnik neni konvexni.\n");
    exit (EXIT_FAILURE);
}


static
void
move_point (point_type * p, vector_type const * v, point_type const * o)
{
    p->x = v->x + o->x;
    p->y = v->y + o->y;
    p->z = v->z + o->z;
}


static
int
coord_signum (coord_type c)
{
    if (c != 0)
        return c < 0 ? -1 : 1;
    else
        return 0;
}


struct context_tag;


typedef struct context_tag context_type;


typedef void (* point_handling_func_type) (context_type *, point_type const *);


struct context_tag
{
    /* Tested point. */
    point_type p_test;

    /* Tested point offset from (0,0,0). */
    vector_type offset;

    /* First vertex of polygon. */
    point_type pfirst;

    /* Current polygon vertex. */
    point_type p;

    /* Previous polygon vertex. */
    point_type pprev;

    /* Previous edge vector. */
    vector_type vprev;

    /* Point count. */
    int n;
    
    /* Angle signum */
    int anglesig;

    /* 1 = non-convex, 0 = convex */
    int non_convex;

    /* Winding number. */
    int wn;

    /* Point handler. */
    point_handling_func_type handler;

};


/* This function computes winding number as described in "A Winding
   Number and Point-in-Polygon Algorithm" by David G. Alciatore.  See
   <http://www.engr.colostate.edu/~dga/dga/papers/point_in_polygon.pdf>.

   The only difference from the paper is that we compare r greater or
   equal to zero to exclude point on polygon boundary from the
   polygon.  */
static
void
compute_wn (context_type * ctx)
{
    point_type const * p = &ctx->p;
    point_type const * pprev = &ctx->pprev;

    assert (ctx->n >= 2);

    if (coord_signum (ctx->pprev.y) * coord_signum (ctx->p.y) < 0)
    {
        double r = pprev->x + ((double)pprev->y * (p->x - pprev->x)) / (pprev->y - p->y);
        if (r >= 0)
        {
            if (pprev->y < 0)
                ctx->wn += WN_UNIT;
            else
                ctx->wn -= WN_UNIT;
        }
    }
    else if (pprev->y == 0 && pprev->x > 0)
    {
        if (p->y > 0)
            ctx->wn += WN_UNIT / 2;
        else
            ctx->wn -= WN_UNIT / 2;
    }
    else if (p->y == 0 && p->x > 0)
    {
        if (pprev->y < 0)
            ctx->wn += WN_UNIT / 2;
        else
            ctx->wn -= WN_UNIT / 2;
    }

    /*printf ("wn: %d\n", ctx->wn);*/
}


static void context_state_0 (context_type * ctx, point_type const * p);
static void context_state_1 (context_type * ctx, point_type const * p);
static void context_state_2 (context_type * ctx, point_type const * p);


void
context_state_0 (context_type * ctx, point_type const * p)
{
    assert (ctx->n == 0);
    
    /* Remember the starting point of the polygon untranslated. It
       will be used also as the last point to close the polygon
       curve. */

    ctx->pfirst = *p;

    /* First point, translate it and remember it. */

    ctx->n = 1;
    move_point (&ctx->p, &ctx->offset, p);
    
    /* Change next state. */

    ctx->handler = context_state_1;
}


void
context_state_1 (context_type * ctx, point_type const * p)
{
    vector_type v;

    assert (ctx->n == 1);

    /* Second point, translate it and remember it. */

    ctx->n = 2;
    ctx->pprev = ctx->p;
    move_point (&ctx->p, &ctx->offset, p);

    /* Update winding number. */

    compute_wn (ctx);
    
    /* Compute first edge's vector and remember it. */

    init_vector (&v, &ctx->pprev, &ctx->p);
    ctx->vprev = v;

    /* Change next state */

    ctx->handler = context_state_2;
}


void
context_state_2 (context_type * ctx, point_type const * p)
{
    vector_type v;
    vector_type w;
    coord_type wzsig;

    assert (ctx->n >= 2);

    /* Translate the new point and remember it. */

    ctx->n += 1;
    ctx->pprev = ctx->p;
    move_point (&ctx->p, &ctx->offset, p);

    /* Update the winding number. */

    compute_wn (ctx);

    /* Current edge's vector. */

    init_vector (&v, &ctx->pprev, &ctx->p);

    /* Check the angle between last two edges using vector cross
       product. The signum must not change if the polygon is to be
       convex polygon. */

    cross_prod (&w, &ctx->vprev, &v);
    if (w.z != 0)
        wzsig = coord_signum (w.z);
    else
        ctx->non_convex = 1;
    
    if (ctx->anglesig == 0)
        ctx->anglesig = wzsig;
    
    if (ctx->anglesig != wzsig)
        ctx->non_convex = 1;

    /* Remember this vector as previous for next round. */

    ctx->vprev = v;
}



static
void
init_context (context_type * ctx, point_type const * p_test)
{
    memset (ctx, 0, sizeof (*ctx));
    ctx->handler = context_state_0;
    ctx->p_test = *p_test;
    init_vector (&ctx->offset, p_test, &origin);
}


static
void
add_polygon_point (context_type * ctx, point_type const * p)
{
    ctx->handler (ctx, p);
}


static
void
end_polygon (context_type * ctx)
{
    ctx->handler (ctx, &ctx->pfirst);

    /* Fix the point count, the first point is counted twice. */

    ctx->n -= 1;

    if (ctx->n < 3)
        bad_input_error ();
}


int
main (void)
{
    int ret;

    point_type p_test;
    point_type p;

    context_type ctx;
    
    printf ("Zadejte souradnice bodu:\n");
    ret = read_point (&p_test);
    if (! ret)
        bad_input_error ();

    init_context (&ctx, &p_test);

    printf ("Zadejte body n-uhelniku:\n");
    while ((ret = read_point (&p)))
    {
        add_polygon_point (&ctx, &p);
    }

    if (ctx.n < 3)
        bad_input_error ();

    if (ctx.non_convex)
        non_convex_error ();

    end_polygon (&ctx);

    /*printf ("wn: %d\n", ctx.wn);*/

    if (ctx.wn == 0)
        printf ("Bod nelezi uvnitr n-uhelniku.\n");
    else
        printf ("Bod lezi uvnitr n-uhelniku.\n");

    return EXIT_SUCCESS;
}
