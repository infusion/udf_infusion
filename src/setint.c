#include "common.h"


my_bool setint_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (4 != args->arg_count) {
        strcpy(message, "setint must have exactly four arguments");
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

longlong setint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2] || NULL == args->args[3]) {
        *is_null = 1;
        return 0;
    }
    longlong n = *((longlong *) args->args[0]);
    longlong x = *((longlong *) args->args[1]);
    longlong y = *((longlong *) args->args[2]);
    longlong m = *((longlong *) args->args[3]);

    return n & (~(((2 << (y - x)) - 1) << x)) | (m << x);
}
