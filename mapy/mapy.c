#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>


void
input_error ()
{
    printf ("Nespravny vstup.\n");
    exit (EXIT_FAILURE);
}


void
alloc_error ()
{
    printf ("Chyba alokace.\n");
    exit (EXIT_FAILURE);
}


enum constants
{
    CITY_NAME_LEN = 30,
    CITY_NAME_BUF_LEN = CITY_NAME_LEN + 1 + 1
};


struct city_name_tag
{
    char n[CITY_NAME_BUF_LEN];
};


typedef struct city_name_tag city_name_type;


struct edge_tag
{
    city_name_type u;
    city_name_type v;
};


typedef struct edge_tag edge_type;


void
print_edge (edge_type const * e)
{
    printf ("{u: %s; v: %s}", e->u.n, e->v.n);
}


struct edge_list_tag
{
    size_t cap;
    size_t len;
    edge_type * buf;
};


typedef struct edge_list_tag edge_list_type;


int
edge_list_init (edge_list_type * l, size_t cap)
{
    l->buf = (edge_type *) malloc (cap * sizeof (edge_type));
    if (l->buf == NULL)
        return 0;

    l->len = 0;
    l->cap = cap;
    
    return 1;
}


int
edge_compare (edge_type const * a, edge_type const * b)
{
    return memcmp (a, b, sizeof (edge_type));
}


edge_type *
edge_list_ubound (edge_list_type * l, edge_type const * e)
{
    ptrdiff_t len = (ptrdiff_t)l->len;
    edge_type * first = l->buf;
    edge_type * mid;

    printf ("ubound e: ");
    print_edge (e);
    printf ("\n");
    
    while (len > 0)
    {
        int cmp;
        ptrdiff_t half;
        
        half = len / 2;
        mid = first + half;

        printf ("first: ");
        print_edge (first);
        printf ("\n");

        printf ("mid: ");
        print_edge (mid);
        printf ("\n");

        cmp = edge_compare (e, mid);
        if (cmp < 0)
            len = half;
        else
        {
            first = mid;
            ++first;
            len = len - half - 1;
        }
    }

    return first;
}



edge_type *
edge_list_get_end (edge_list_type * l)
{
    return l->buf + l->len;
}


int
edge_list_realloc (edge_list_type * l, size_t new_cap)
{
    edge_type * new_buf;

    assert (new_cap >= l->cap);
    new_buf = realloc (l->buf, new_cap * sizeof (edge_type));
    if (new_buf == NULL)
        return 0;

    l->buf = new_buf;
    l->cap = new_cap;

    return 1;
}


int
edge_list_append (edge_list_type * l, edge_type const * e)
{
    if (l->len == l->cap)
        if (! edge_list_realloc (l, l->cap * 2 + 1))
            return 0;

    l->buf[l->len] = *e;
    ++l->len;

    return 1;
}



int
edge_list_insert (edge_list_type * l, edge_type const * e)
{
    edge_type * it;
    edge_type * last = edge_list_get_end (l);

    it = edge_list_ubound (l, e);
    if (it == last)
    {
        /* Not in the list yet and it should be placed at the very
           end. */
        return edge_list_append (l, e);
    }

    else if (edge_compare (it, e) == 0)
        /* This edge already exists in the list. */
        return 0;

    else
    {
        /* Insert in the middle. */
        memmove (it + 1, it, (last - it) * sizeof (edge_type));
        *it = *e;
        ++l->len;
        return 1;
    }
}


int
read_name (char * buf, size_t * out_len, size_t len)
{
    int ch;
    size_t i = 0;

    while ((ch = fgetc (stdin)) != EOF && i < len)
    {               
        if (! isalpha ((unsigned char)ch))
        {
            int ret = ungetc (ch, stdin);
            if (ret == EOF)
                return 0;
            
            break;
        }

        buf[i] = ch;
        ++i;
    }
    
    if (ch == EOF)
        return ch;

    buf[i] = 0;
    *out_len = i;

    return 1;
}


int
consume_space (void)
{
    int ch;

    while ((ch = fgetc (stdin)) != EOF
        && ch == ' ')
        ;

    if (ch != EOF)
    {
        int ret = ungetc (ch, stdin);
        if (ret == EOF)
            input_error ();
    }

    return ch;
}


int
consume_newline (void)
{
    int ch;

    ch = fgetc (stdin);
    if (ch == EOF)
        return ch;

    if (ch == '\r')
    {
        ch = fgetc (stdin);
        if (ch == EOF)
            return ch;
    }
    
    if (ch == '\n')
        ;
    else
    {
        int ret = ungetc (ch, stdin);
        if (ret == EOF)
            input_error ();
    }

    return ch;
}


int
read_sep ()
{
    int ret;

    ret = consume_space ();
    if (ret == EOF)
        return 0;

    ret = fgetc (stdin);
    if (ret == EOF || ret != '-')
        return ret;

    ret = consume_space ();
    if (ret == EOF)
        return 0;

    return 1;
}


int
read_into_edge_list (edge_list_type * l)
{
    edge_type e;
    city_name_type city;
    size_t name_len;
    int ret;

    while (1)
    {
        memset (&e, 0, sizeof (e));
        ret = read_name (e.u.n, &name_len, CITY_NAME_BUF_LEN);

        if (ret == EOF)
            return ret;

        if (! ret)
            return 0;

        if (name_len == 0)
            return 1;
        
        if (name_len > CITY_NAME_LEN)
            input_error ();

        if (! read_sep ())
            input_error ();

        ret = read_name (e.v.n, &name_len, CITY_NAME_BUF_LEN);
        if (name_len > CITY_NAME_LEN)
            input_error ();

        ret = consume_space ();
        if (ret == EOF)
            return ret;

        ret = consume_newline ();
        if (ret == EOF)
            return ret;

        ret = strcmp (e.u.n, e.v.n);
        if (ret == 0)
            /* Looping edge. */
            input_error ();
        else if (ret > 0)
        {
            /* Swap the edges so that we get a canonical order so that
               we can compare edges for equality easier. */
            city = e.u;
            e.u = e.v;
            e.v = city;
        }

        ret = edge_list_insert (l, &e);
        if (! ret)
            input_error ();
    }
}


int
main ()
{
    edge_list_type l1;
    edge_list_type l2;
    int ret;

    printf ("Zadejte cesty:\n");

    if (! edge_list_init (&l1, 10))
        alloc_error ();

    ret = read_into_edge_list (&l1);
    if (ret == EOF || ret == 0)
        input_error ();

    if (! edge_list_init (&l2, 10))
        alloc_error ();

    ret = consume_newline ();
    if (ret == EOF)
        input_error ();

    ret = read_into_edge_list (&l2);
    if (ret != EOF)
        input_error ();

    return EXIT_SUCCESS;
}
