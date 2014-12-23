#include "common.h"

#define XCHNG(a,b) { register double tmp = buf[a]; buf[a] = buf[b]; buf[b] = tmp; }

/*
 *  This algorithm is based on the description in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 */
static double __median(double *buf, int len)
{
    int low = 0,
            high = len - 1,
            median = high / 2,
            mid, ll, hh;

    do {

        if (high <= low)
            return buf[median];

        if (high == low + 1) {
            if (buf[high] < buf[low])
                XCHNG(low, high);
            return buf[median];
        }

        ll = low + 1;
        hh = high;

        mid = (low + high) / 2;
        if (buf[mid] > buf[high]) XCHNG(mid, high);
        if (buf[low] > buf[high]) XCHNG(low, high);
        if (buf[mid] > buf[low])  XCHNG(mid, low);

        XCHNG(mid, ll);

        do {
            do ++ll; while (buf[low] > buf[ll]);
            do --hh; while (buf[hh] > buf[low]);

            if (hh < ll)
                break;

            XCHNG(ll, hh);
        } while (1);

        XCHNG(low, hh);

        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;

    } while (1);
}
#undef XCHNG

my_bool median_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    struct DoubleBuffer* data = NULL;

    if (1 != args->arg_count) {
        strcpy(message, "median must have exactly one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;

    LESSINIT();

    return 0;
}

void median_clear(UDF_INIT* initid, char* is_null, char *error)
{
    LESSCLEAR();
}

void median_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

    if (NULL == args->args[0])
        return;

    double value = *((double*) args->args[0]);

    LESSSIZE();

    data->number[data->used] = value;
    data->used++;
}

void median_deinit(UDF_INIT *initid)
{
    LESSDEINIT();
}

double median(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

    if (data->used==0) {
        *is_null = 1;
        return 0;
    }
    return __median(data->number, data->used);
}
