#define NDEBUG

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#if defined (DMALLOC)
#include <dmalloc.h>
#endif


#define BUF_MIN_CAP (16)

#define HANDLE_ERROR(msg) do { \
    error_msg = (msg);         \
    ec = EXIT_FAILURE;         \
    goto error;                \
} while (0)


static char const input_error[] = "Nespravny vstup.";
static char const alloc_error[] = "Chyba alokace.";


struct buf
{
    char * b;
    size_t len;
    size_t cap;
};


typedef struct buf buf_type;


#if 0
static
void
buf_construct (buf_type * buf, char * mem, size_t l, size_t c)
{
    assert (mem != NULL);
    assert (l <= c);
    assert (c > 0);
    assert (buf->b == NULL);

    buf->b = mem;
    buf->len = l;
    buf->cap = c;
}
#endif


static
buf_type
buf_new (size_t len)
{
    buf_type buf;
    size_t cap;
    char * mem;

    if (len < BUF_MIN_CAP)
        cap = BUF_MIN_CAP;
    else
        cap = len;

    mem = (char *)malloc (cap);
    if (mem == NULL)
    {
        exit (EXIT_FAILURE);
        /*abort ();*/
    }
    memset (mem, 0, cap);
    memset (&buf, 0, sizeof (buf));

    buf.b = mem;
    buf.len = len;
    buf.cap = cap;

    return buf;
}


static
void
buf_destroy (buf_type const * buf)
{
    if (buf)
    {
        free (buf->b);
        memset (&buf, 0, sizeof (buf));
    }
}


static
void
buf_zero (buf_type * buf)
{
    memset (buf, 0, sizeof (*buf));
}


static
void
buf_realloc (buf_type * buf, size_t new_cap)
{
    char * new_mem = (char *)realloc (buf->b, new_cap);
    if (new_mem == NULL)
    {
        free (buf->b);
        exit (EXIT_FAILURE);
        /*abort ();*/
    }

    buf->b = new_mem;
    buf->cap = new_cap;
    if (buf->len > buf->cap)
        buf->len = buf->cap;
}


static
void
buf_resize (buf_type * buf, size_t new_len)
{
    if (new_len > buf->cap)
        buf_resize (buf, new_len);
    buf->len = new_len;
}


static
void
buf_append (buf_type * buf, char ch)
{
    assert (buf);

    if (buf->len == buf->cap)
        buf_realloc (buf, buf->cap + 100);

    buf->b[buf->len] = ch;
    ++buf->len;
}


static
size_t
buf_getlen (buf_type const * buf)
{
    return buf->len;
}


static
char const *
buf_getdata_const (buf_type const * buf)
{
    return buf->b;
}


static
void
buf_move (buf_type * dest, buf_type * src)
{
    if (dest != src)
    {
        buf_destroy (dest);
        *dest = *src;
        buf_zero (src);
    }
}


#if 0
void
consume_ws ()
{
    int tmp;
    while ((tmp = fgetc (stdin)) != EOF
        && (isspace (tmp) || iscntrl (tmp)))
        ;
    if (tmp != EOF)
        ungetc (tmp, stdin);
}
#endif


static
int
read_row (buf_type * line_buf)
{
    while (1)
    {
        int ch = fgetc (stdin);

        if ((char)ch == '\n')
            return ch;

        if (ch == EOF)
            return ch;

        if (! isdigit (ch))
            return 0;

        /* Just another digit, append to buffer. */

        buf_append (line_buf, (char)ch);
    }
}


static
void
dump_matrix (buf_type const * m, size_t rows, size_t cols)
{
    size_t i = 0;

    (void)m;
    (void)rows;
    (void)cols;
    (void)i;

#if defined (DEBUG)
    for (i = 0; i != rows; ++i)
    {
        char const * const it = buf_getdata_const (&m[i]);
        fwrite (it, 1, cols, stdout);
        printf ("\n");
    }
    printf ("---\n");
#endif
}


static
int
search_haystack (buf_type const * haystack, buf_type const * needle,
    size_t rows)
{
    size_t const haystack_columns = buf_getlen (haystack);
    size_t const needle_columns = buf_getlen (needle);
    size_t haystack_c, row;
    int count = 0;

    if (haystack_columns < needle_columns)
        return 0;

    dump_matrix (haystack, rows, haystack_columns);

    for (haystack_c = 0; haystack_c + needle_columns <= haystack_columns;
         ++haystack_c)
    {
        int found = 1;

        assert (haystack_c < haystack_columns);

        for (row = 0; row != rows; ++row)
        {
            int ret;
            char const * const haystack_row = buf_getdata_const (&haystack[row]);
            char const * const haystack_row_it = haystack_row + haystack_c;

            char const * const needle_row_it = buf_getdata_const (&needle[row]);

            ret = memcmp (needle_row_it, haystack_row_it, needle_columns);

            if (ret != 0)
            {
                found = 0;
                break;
            }
        }

        if (found)
            ++count;
    }

    return count;
}


static
void
free_m (buf_type const * buf, size_t n)
{
    if (buf)
    {
        size_t i;
        for (i = 0; i != n; ++i)
            buf_destroy (&buf[i]);
        free ((void *)buf);
    }
}


int
main ()
{
    char const * error_msg = 0;
    int ret = 0;
    int ec = EXIT_SUCCESS;
    int needle_rows = 0;
    int needle_columns = -1;
    int needle_r = 0;
    buf_type * needle = 0;
    buf_type line_buf;

    buf_type * haystack = 0;
    int haystack_columns = -1;
    int haystack_r = 0;

    int match_count = 0;

    memset (&line_buf, 0, sizeof (line_buf));

    printf ("Zadejte pocet radek hledane matice:\n");
    ret = scanf ("%d", &needle_rows);
    if (ret != 1 || needle_rows <= 0)
        HANDLE_ERROR (input_error);

    needle = (buf_type *)malloc (needle_rows * sizeof (buf_type));
    if (needle == NULL)
        HANDLE_ERROR (alloc_error);

    memset (needle, 0, needle_rows * sizeof (buf_type));

    printf ("Zadejte hledanou matici:\n");

    /* Consume EOL from the previous line. */

    (void)fgetc (stdin);

    /* Read needle from the standard input. */

    for (needle_r = 0; needle_r != needle_rows; ++needle_r)
    {
        int line_buf_len;

        line_buf = buf_new (0);

        ret = read_row (&line_buf);
        if (ret == EOF || ret == 0)
            HANDLE_ERROR (input_error);

        /* Either remember number of columns or check that this row
           has the same lenght as all previous rows.  */

        line_buf_len = (int)buf_getlen (&line_buf);

        if (needle_columns == -1)
            needle_columns = line_buf_len;
        else if (needle_columns != line_buf_len)
            HANDLE_ERROR (input_error);

        buf_move (&needle[needle_r], &line_buf);
    }

    /* We only need a window as tall as the needle into the
       haystack.  */

    haystack = (buf_type *)malloc (needle_rows * sizeof (buf_type));
    if (haystack == NULL)
        HANDLE_ERROR (alloc_error);

    memset (haystack, 0, needle_rows * sizeof (buf_type));

    /* Main loop reading haystack and looking for needle in it. */

    printf ("Matice, ktera bude prohledana:\n");

    while (1)
    {
        int line_buf_len;

        if (haystack_r < needle_rows)
        {
            line_buf = buf_new (0);
            buf_zero (&haystack[haystack_r]);
        }
        else
            buf_move (&line_buf, &haystack[haystack_r - 1]);

        ret = read_row (&line_buf);
        if (ret == EOF)
            break;

        /* Either remember number of columns or check that this row
           has the same length as all previous rows.  */

        line_buf_len = (int)buf_getlen (&line_buf);
        if (line_buf_len == 0)
            HANDLE_ERROR (input_error);

        if (haystack_columns == -1)
            haystack_columns = line_buf_len;
        else if (haystack_columns != line_buf_len)
            HANDLE_ERROR (input_error);

        /*if (haystack_columns < needle_columns)
          input_error ();*/

        if (haystack_r < needle_rows)
        {
            buf_move (&haystack[haystack_r], &line_buf);
            ++haystack_r;
        }
        else
            buf_move (&haystack[haystack_r - 1], &line_buf);

        if (haystack_r == needle_rows)
        {
            buf_type tmp_buf;
            int count;

            /* Search the haystack window for needle. */

            count = search_haystack (haystack, needle, (size_t)needle_rows);
            match_count += count;

            /* Shift haystack window, "forget" the oldest row. */

            if (haystack_r > 1)
            {
                tmp_buf = haystack[0];
                memmove (&haystack[0], &haystack[1],
                    (haystack_r - 1) * sizeof (buf_type));
                haystack[haystack_r - 1] = tmp_buf;
            }
            buf_resize (&haystack[haystack_r - 1], 0);
        }
    }

    if (haystack_r > 0)
        printf ("Matice nalezena %d krat.\n", match_count);
    else
        HANDLE_ERROR (input_error);

    ec = EXIT_SUCCESS;
    goto ok;

error:
    if (error_msg != NULL)
        puts (error_msg);

ok:
    free_m (haystack, (size_t)needle_rows);
    free_m (needle, (size_t)needle_rows);
    buf_destroy (&line_buf);

    return ec;
}
