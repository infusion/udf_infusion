#include "common.h"


my_bool rsumd_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    double *data;

    if (1 != args->arg_count) {
        strcpy(message, "rsumd must have one argument");
        return 1;
    }

    if (NULL == (data = malloc(sizeof (*data)))) {
        strcpy(message, "Couldn't allocate memory");
        return 1;
    }
    *data = 0;

    args->arg_type[0] = REAL_RESULT;

    initid->ptr = (char *) data;
    initid->const_item = 0;
    initid->maybe_null = 1;

    return 0;
}

double rsumd(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    double *data;

    if (initid->ptr) {
        data = (double *) initid->ptr;

        if (NULL == args->args[0]) {
            return *data;
        }

        (*data)+= *((double *) args->args[0]);

        return *data;
    }
    *error = 1;
    return 0;
}

void rsumd_deinit(UDF_INIT *initid)
{
    if (initid->ptr) {
        free(initid->ptr);
    }
}
