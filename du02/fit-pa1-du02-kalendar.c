#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define GREGORIAN_CALENDAR_YEAR ((year_type)1584)
#define YEAR_DAYS ((daycount_type)365)
#define LEAP_YEAR_DAYS ((daycount_type)366)
#define GREGORIAN_DAYCOUNT_FIX ((daycount_type)17 - 1)
#define TOTAL_JULIAN_LEAP_YEARS ((GREGORIAN_CALENDAR_YEAR - 1) / 4)
#define TOTAL_JULIAN_NONLEAP_YEARS (GREGORIAN_CALENDAR_YEAR - TOTAL_JULIAN_LEAP_YEARS)
#define GREGORIAN_LEAP_YEARS_FIX                 \
    ((GREGORIAN_CALENDAR_YEAR - 1) / 4           \
        - (GREGORIAN_CALENDAR_YEAR - 1) / 100    \
        + (GREGORIAN_CALENDAR_YEAR - 1) / 400    \
        - (GREGORIAN_CALENDAR_YEAR - 1) / 4000)
#define GREGORIAN_NON_LEAP_YEARS_FIX ((GREGORIAN_CALENDAR_YEAR - 1) - GREGORIAN_LEAP_YEARS_FIX)


#if defined (__GNUC__)
typedef unsigned long long big_uint_type;
#define BI_FMT "%llu"

#elif defined (_MSC_VER)
typedef unsigned __int64 big_uint_type;
#define BI_FMT "%I64u"

#else
typedef unsigned long big_uint_type;
#define BI_FMT "%lu"

#endif


typedef big_uint_type daycount_type;
#define DC_FMT BI_FMT


typedef big_uint_type year_type;
#define Y_FMT BI_FMT


typedef unsigned short month_type;
typedef unsigned short day_type;


struct date_tag
{
    year_type year;
    month_type month;
    day_type day;
};


typedef struct date_tag date_type;


static const day_type days_in_month_tbl[2][13] = {
    /* Non-leap year. */
    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    /* Leap year. */
    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
};


static const daycount_type cum_days_in_month_tbl[2][13] = {
    /* Non-leap year. */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap year. */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};


static const date_type first_valid_date = { 1, 1, 1 };


static const date_type last_julian_date = { 1584, 1, 6 };


static const date_type first_gregorian_date = { 1584, 1, 17 };


static
int
date_eq (date_type const * d1, date_type const * d2)
{
    return memcmp (d1, d2, sizeof (*d1)) == 0;
}


static
int
date_lt (date_type const * d1, date_type const * d2)
{
    if (d1->year == d2->year)
    {
        if (d1->month == d2->month)
            return d1->day < d2->day;
        else
            return d1->month < d2->month;
    }
    else
        return d1->year < d2->year;
}


static
int
date_gt (date_type const * d1, date_type const * d2)
{
    return date_lt (d2, d1);
}


static
int
date_lte (date_type const * d1, date_type const * d2)
{
    return ! date_gt (d1, d2);
}


static
int
date_gte (date_type const * d1, date_type const * d2)
{
    return ! date_lt (d1, d2);
}


static
void
set_date (date_type * date, day_type day, month_type month, year_type year)
{
    date->day = day;
    date->month = month;
    date->year = year;
}



static
int
days_in_month (month_type m, int leap_year)
{
    assert (1 <= m && m <= 12);
    assert (leap_year == 0 || leap_year == 1);

    return days_in_month_tbl[leap_year][m];
}


static
int
is_leap_year (year_type y)
{
    int is_leap;

    assert (y > 0);

    if (y < GREGORIAN_CALENDAR_YEAR)
    {
        /* Julian calendar. */

        is_leap = y % 4 == 0;
    }
    else
    {
        /* Gregorian calendar. */

        if (y % 4000 == 0)
            is_leap = 0;
        else if (y % 400 == 0)
            is_leap = 1;
        else if (y % 100 == 0)
            is_leap = 0;
        else
            is_leap = y % 4 == 0;
    }

    return is_leap;
}


static
int
is_valid_date (date_type const * d)
{
    return (1 <= d->year)
        && (1 <= d->month && d->month <= 12)
        && (1 <= d->day
            && d->day <= days_in_month (d->month, is_leap_year (d->year)))
        && ((date_lte (&first_valid_date, d) && date_lte (d, &last_julian_date))
            || date_lte (&first_gregorian_date, d));
}


static
int
read_date (date_type * date)
{
    int ret;
    int day, month;
    year_type year;

    assert (date);

    ret = scanf ("%d %d " Y_FMT, &day, &month, &year);
    if (ret != 3)
        goto error;

    set_date (date, day, month, year);
    if (! is_valid_date (date))
        goto error;

    return 1;

error:
    printf ("Nespravny vstup.\n");
    return 0;
}


static
int
print_date_f (FILE * f, date_type const * d)
{
    int ret = fprintf (f, "%u.%u." Y_FMT, d->day, d->month, d->year);
    return ret;
}


static
int
print_date (date_type const * d)
{
    return print_date_f (stdout, d);
}


static
daycount_type
absolute_julian_daycount (date_type const * d)
{
    year_type ym1 = d->year - 1;
    year_type leap_years = ym1 / 4;
    year_type non_leap_years = ym1 - leap_years;
    daycount_type whole_years_days
        = non_leap_years * YEAR_DAYS
        + leap_years * LEAP_YEAR_DAYS;
    int date_year_is_leap = is_leap_year (d->year);
    daycount_type date_year_days
        = cum_days_in_month_tbl[date_year_is_leap][d->month - 1]
        + d->day
        - 1;
    daycount_type daycount = whole_years_days + date_year_days;

    assert (is_valid_date (d) && date_lte (d, &last_julian_date));

    return daycount;
}


static
daycount_type
absolute_gregorian_daycount (date_type const * d)
{
    year_type y = d->year;
    year_type ym1 = y - 1;
    year_type leap_years_raw
        = ym1 / 4
        - ym1 / 100
        + ym1 / 400
        - ym1 / 4000;

    year_type leap_years
        = leap_years_raw
        - GREGORIAN_LEAP_YEARS_FIX;

    year_type non_leap_years
        = ym1
        - leap_years_raw
        - GREGORIAN_NON_LEAP_YEARS_FIX;

    daycount_type whole_years_days
        = non_leap_years * YEAR_DAYS
        + leap_years * LEAP_YEAR_DAYS;

    int date_year_is_leap = is_leap_year (y);
    daycount_type date_year_days
        = cum_days_in_month_tbl[date_year_is_leap][d->month - 1]
        + d->day;

    daycount_type daycount
        = whole_years_days
        + date_year_days
        - GREGORIAN_DAYCOUNT_FIX;

    assert (is_valid_date (d) && date_gte (d, &first_gregorian_date));

    return daycount;
}


static
daycount_type
absolute_daycount (date_type const * d)
{
    daycount_type julian_daycount;
    daycount_type gregorian_daycount;

    assert (is_valid_date (d));

    if (date_gte (d, &first_gregorian_date))
    {
        julian_daycount = absolute_julian_daycount (&last_julian_date);
        gregorian_daycount = absolute_gregorian_daycount (d);
    }
    else
    {
        julian_daycount = absolute_julian_daycount (d);
        gregorian_daycount = 0;
    }

    return julian_daycount + gregorian_daycount;
}


static
daycount_type
count_days (date_type const * d1, date_type const * d2)
{
    return absolute_daycount (d2) - absolute_daycount (d1);
}


int
main (void)
{
    date_type d1, d2;
    int ret;

    puts ("Zadejte prvni datum - den mesic rok:");
    ret = read_date (&d1);
    if (! ret)
        return EXIT_FAILURE;

    puts ("Zadejte druhe datum - den mesic rok:");
    ret = read_date (&d2);
    if (! ret)
        return EXIT_FAILURE;

    if (date_lt (&d2, &d1))
    {
        printf ("Prve datum je vetsi nez datum druhe.\n");
        return EXIT_FAILURE;
    }

    fputs ("Mezi ", stdout);
    print_date (&d1);
    fputs (" a ", stdout);
    print_date (&d2);
    printf (" ubehlo " DC_FMT " dni.\n", count_days (&d1, &d2));

    return EXIT_SUCCESS;
}
