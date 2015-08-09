#include "common.h"

my_bool noverk_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    if (2 != args->arg_count) {
        strcpy(message, "noverk must have exactly two arguments");
        return 1;
    }

    args->arg_type[0] = INT_RESULT;
    args->arg_type[1] = INT_RESULT;
    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = NOT_FIXED_DEC;
    initid->max_length = 20;

    return 0;
}

longlong noverk(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {

    if (NULL == args->args[0] || NULL == args->args[1]) {
        *is_null = 1;
        return 0;
    }

    longlong i, res = 1;

    longlong n = *((longlong *) args->args[0]);
    longlong k = *((longlong *) args->args[1]);

    if (!(k > 0 && n > 0 && n > k)) {
        *is_null = 1;
        return 0;
    }

    n = n - k;

    for (i = 1; i <= k; i++) {
        res = res * (n + i) / i; // Simplified from res = res * (n - i + 1) / i
    }

    return res;
}
