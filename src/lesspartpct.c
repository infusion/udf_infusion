#include "common.h"

DLLEXPORT my_bool lesspartpct_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct DoubleBuffer* data = NULL;

    if (2 != args->arg_count) {
        strcpy(message, "lesspartpct must have exaclty two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;

    LESSINIT();

    return 0;
}

DLLEXPORT void lesspartpct_clear(UDF_INIT* initid, char* is_null, char *error) {
    LESSCLEAR();
}

DLLEXPORT void lesspartpct_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    int type = 1;
    LESSADD();
}

DLLEXPORT void lesspartpct_deinit(UDF_INIT *initid) {
    LESSDEINIT();
}

DLLEXPORT longlong lesspartpct(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
    double limit = *((double*) args->args[1]);

    double psum = 0;

    ulonglong count = 0;

    if (!(limit >= 0 && limit <= 1)) {
        *error = 1;
        return 0;
    }

    limit = limit * data->sum;

    LESS();

    return (longlong) count;
}
