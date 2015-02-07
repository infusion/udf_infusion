#include "common.h"


my_bool slug_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (1 != args->arg_count) {
        strcpy(message, "slug must have exaclty one argument");
        return 1;
    }

    args->arg_type[0] = STRING_RESULT;

    initid->max_length = args->attribute_lengths[0] * 2;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

char *slug(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused)))
{
    if (NULL == args->args[0]) {
        *is_null = 1;
        return 0;
    }

    return _translate_string(args, result, length, '-');
}
