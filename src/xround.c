#include "common.h"


my_bool xround_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (1 != args->arg_count) {
        strcpy(message, "xround must have exactly one single argument");
        return 1;
    }

    switch (args->arg_type[0]) {
    case STRING_RESULT:
    case DECIMAL_RESULT:
        args->arg_type[0] = REAL_RESULT;
    default:
        break;
    }
    initid->const_item = 1;
    initid->maybe_null = 0;

    return 0;
}

#define RET_NEG(s) return (((s) ^ -f) + f)
longlong xround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{
    longlong n, f = 0;
    double d;

    if (NULL == args->args[0]) {
        *is_null = 1;
        return 0;
    }

    if (INT_RESULT == args->arg_type[0]) {

        n = *((longlong *) args->args[0]);

    } else {

        d = *((double *) args->args[0]);
        n = (longlong) d;

        n = n + (n < d) - (d < n);
    }

    if (n < 0) {
        n = -n;
        f = 1;
    }

    if (n > 1000000000LL) {

        if (n > 100000000000000LL) {
            if (n > 10000000000000000LL) {
                if (n <= 100000000000000000LL) RET_NEG(100000000000000000LL);
                if (n <= 1000000000000000000LL) RET_NEG(1000000000000000000LL);
                RET_NEG(1000000000000000000LL);
            } else {
                if (n <= 1000000000000000LL) RET_NEG(1000000000000000LL);
                RET_NEG(10000000000000000LL);
            }
        } else {
            if (n > 100000000000LL) {
                if (n <= 1000000000000LL) RET_NEG(1000000000000LL);
                if (n <= 10000000000000LL) RET_NEG(10000000000000LL);
                RET_NEG(100000000000000LL);
            } else {
                if (n <= 10000000000LL) RET_NEG(10000000000LL);
                RET_NEG(100000000000LL);
            }
        }
    } else if (n > 10000LL) {
        if (n > 1000000LL) {
            if (n <= 10000000LL) RET_NEG(10000000LL);
            if (n <= 100000000LL) RET_NEG(100000000LL);
            RET_NEG(1000000000LL);
        } else {
            if (n <= 100000LL) RET_NEG(100000LL);
            RET_NEG(1000000LL);
        }
    } else {
        if (n > 100LL) {
            if (n <= 1000LL) RET_NEG(1000LL);
            RET_NEG(10000LL);
        } else {
            if (n <= 1LL) RET_NEG(1LL);
            if (n <= 10LL) RET_NEG(10LL);
            RET_NEG(100LL);
        }
    }
    RET_NEG(1LL);
}
#undef RET_NEG
