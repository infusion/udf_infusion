#include "common.h"


my_bool bound_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (3 != args->arg_count) {
        strcpy(message, "bound must have exactly three arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    args->arg_type[2] = REAL_RESULT;

    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = NOT_FIXED_DEC;
    initid->max_length = 20;

    return 0;
}

double bound(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{
    double *n, *x, *y;

    n = (double *) args->args[0];
    x = (double *) args->args[1];
    y = (double *) args->args[2];

    if (NULL == n) {
        *is_null = 1;
        return 0;
    }

    if (NULL != y && NULL != x && *y < *x) {
        return *x;
    }

    if (NULL != y && *y < *n) {
        return *y;
    }

    if (NULL != x && *n < *x) {
        return *x;
    }
    return *n;
}
