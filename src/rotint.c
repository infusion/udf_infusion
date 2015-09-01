#include "common.h"

DLLEXPORT my_bool rotint_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    if (4 != args->arg_count) {
        strcpy(message, "rotint must have exactly four arguments");
        return 1;
    }

    args->arg_type[0] = INT_RESULT;
    args->arg_type[1] = INT_RESULT;
    args->arg_type[2] = INT_RESULT;
    args->arg_type[3] = INT_RESULT;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

DLLEXPORT longlong rotint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {

    longlong n, x, y, m, a, b;

    if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2] || NULL == args->args[3]) {
        *is_null = 1;
        return 0;
    }

    n = *((longlong *) args->args[0]);
    x = *((longlong *) args->args[1]);
    y = *((longlong *) args->args[2]);
    m = *((longlong *) args->args[3]);

    if (y < x || x < 1) {
        *is_null = 1;
        return 0;
    }

    y-= x;
    m = (y + (m % y)) % y;

    a = (1 << y) - 1;
    b = (n >> x) & a;

    return (n & (~(a << x))) | ((((b << m) | (b >> (y - m))) & a) << x);
}
