#include "common.h"

my_bool lessavg_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct DoubleBuffer* data = NULL;

    if (1 != args->arg_count) {
        strcpy(message, "lessavg must have exactly one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;

    LESSINIT();

    return 0;
}

void lessavg_clear(UDF_INIT* initid, char* is_null, char *error) {
    LESSCLEAR();
}

void lessavg_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
    double value = *((double*) args->args[0]);

    if (NULL == args->args[0])
        return;

    LESSSIZE();

    data->sum = data->sum + value;
    data->number[data->used] = value;
    data->used++;
}

void lessavg_deinit(UDF_INIT *initid) {
    LESSDEINIT();
}

longlong lessavg(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

    ulonglong count = 0;

    double /* volatile */ *ptr;
    double limit = data->sum / data->used;
    double *beg = data->number;

    if (1 == data->used % 2) {
        count = (data->number[--data->used] < limit);
    }

    ptr = &(data->number[data->used]);

    while (beg < ptr) {
        count += (*--ptr < limit);
        count += (*--ptr < limit); // new line saves the volatile pointer
    }
    return (longlong) count;
}
