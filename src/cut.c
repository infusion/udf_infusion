#include "common.h"


my_bool cut_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count && 3 != args->arg_count) {
        strcpy(message, "cut must have two or three arguments");
        return 1;
    }

    args->arg_type[0] = STRING_RESULT;
    args->arg_type[1] = INT_RESULT;
    args->arg_type[2] = STRING_RESULT;

    initid->max_length = args->attribute_lengths[0];
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

char *cut(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused)))
{

    char *str = args->args[0], *c = "...";
    long sl = args->lengths[0], cl = 3, max = *((longlong *) args->args[1]), i;

    if (NULL == str) {
        *is_null = 1;
        return 0;
    }

    if (3 == args->arg_count && NULL != args->args[2]) {
        c = args->args[2];
        cl = args->lengths[2];
    }

    if (max > 0 && (sl - cl) > max) {

        for (i = (sl - 1) < max ? (sl - 1) : max; i >= 0; i--) {
            switch (str[i]) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                goto done;
            }
        }
    done:
        if (-1 == i) {
            *length = sl < max ? sl : max;
        } else {
            *length = i;
        }

        memcpy(result, str, *length);
        memcpy(result + *length, c, cl);
        *length+= cl;
        return result;
    }

    memcpy(result, str, sl);
    *length = sl;
    result[*length] = 0;
    return result;
}
