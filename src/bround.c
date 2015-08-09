#include "common.h"

my_bool bround_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    if (2 != args->arg_count) {
        strcpy(message, "bround must have exactly two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = NOT_FIXED_DEC;
    initid->max_length = 20;

    return 0;
}

double bround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {

    if (NULL == args->args[0] || NULL == args->args[1] || 0 == *((double *) args->args[1])) {
        *is_null = 1;
        return 0;
    }

    return ceil(*((double *) args->args[0]) / *((double *) args->args[1])) * *((double *) args->args[1]);
}
