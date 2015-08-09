#include "common.h"

my_bool lesspart_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct DoubleBuffer* data = NULL;

    if (2 != args->arg_count) {
        strcpy(message, "lesspart must have exaclty two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;

    LESSINIT();

    return 0;
}

void lesspart_clear(UDF_INIT* initid, char* is_null, char *error) {
    LESSCLEAR();
}

void lesspart_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    int type = 0;
    LESSADD();
}

void lesspart_deinit(UDF_INIT *initid) {
    LESSDEINIT();
}

longlong lesspart(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
    double limit = *((double*) args->args[1]);

    double psum = 0;

    ulonglong count = 0;

    if (limit < 0) {
        *error = 1;
        return 0;
    }

    LESS();

    return (longlong) count;
}
