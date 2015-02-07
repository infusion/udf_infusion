#include "common.h"


my_bool rsumi_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    longlong *data;

    if (1 != args->arg_count) {
        strcpy(message, "rsumi must have one argument");
        return 1;
    }

    if (NULL == (data = malloc(sizeof (*data)))) {
        strcpy(message, "Couldn't allocate memory");
        return 1;
    }
    *data = 0;

    args->arg_type[0] = INT_RESULT;

    initid->ptr = (char *) data;
    initid->const_item = 0;
    initid->maybe_null = 1;

    return 0;
}

longlong rsumi(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    longlong *data;

    if (initid->ptr) {
        data = (longlong *) initid->ptr;

        if (NULL == args->args[0]) {
            return *data;
        }

        (*data)+= *((longlong *) args->args[0]);

        return *data;
    }
    *error = 1;
    return 0;
}

void rsumi_deinit(UDF_INIT *initid)
{
    if (initid->ptr) {
        free(initid->ptr);
    }
}
