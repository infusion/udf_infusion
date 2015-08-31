#include "common.h"

DLLEXPORT my_bool setbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    switch (args->arg_count) {
        case 3:
            args->arg_type[2] = INT_RESULT;
        case 2:
            args->arg_type[0] = INT_RESULT;
            args->arg_type[1] = INT_RESULT;
            break;
        default:
            strcpy(message, "setbit must have two or three arguments");
            return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

DLLEXPORT longlong setbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {

    if (NULL == args->args[0] || NULL == args->args[1]) {
        *is_null = 1;
        return 0;
    }

    longlong bit = *((longlong *) args->args[0]);
    longlong n = *((longlong *) args->args[1]);

    if (2 == args->arg_count || (NULL != args->args[2] && 1 == *((longlong *) args->args[2]))) {
        return bit | (1 << n);
    }
    return bit & (~(1 << n));
}
