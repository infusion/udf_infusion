#include "common.h"


my_bool isbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count) {
        strcpy(message, "isbit must have exactly two arguments");
        return 1;
    }

    args->arg_type[0] = INT_RESULT;
    args->arg_type[1] = INT_RESULT;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

longlong isbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{

    if (NULL == args->args[0] || NULL == args->args[1]) {
        *is_null = 1;
        return 0;
    }

    longlong bit = *((longlong *) args->args[0]);
    longlong n = *((longlong *) args->args[1]);

    return (bit >> n) & 1;
}
