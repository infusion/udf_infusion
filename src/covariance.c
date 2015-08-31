#include "common.h"

struct CovBuffer {
    double x;
    double y;
    double c;
    unsigned count;
};

DLLEXPORT my_bool covariance_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct CovBuffer *data;

    if (2 != args->arg_count) {
        strcpy(message, "covariance must have exaclty two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;

    if (NULL == (data = malloc(sizeof (*data)))) {
        strcpy(message, "Memory allocation failed");
        return 1;
    }

    initid->decimals = NOT_FIXED_DEC;
    initid->maybe_null = 1;
    initid->ptr = (char*) data;

    return 0;
}

void covariance_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

    data->count = 0;
    data->x = 0;
    data->y = 0;
    data->c = 0;
}

void covariance_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

    if (NULL == args->args[0] || NULL == args->args[1])
        return;

    double x = *((double *) args->args[0]);
    double y = *((double *) args->args[1]);

    data->count++;

    data->x += x;
    data->y += y;
    data->c += x * y;
}

DLLEXPORT void covariance_deinit(UDF_INIT *initid) {
    struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

    if (data) {
        free(data);
    }
}

DLLEXPORT double covariance(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {
    struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

    if (data->count == 0) {
        *is_null = 1;
        return 0;
    }
    return (data->c - data->x * data->y / data->count) / data->count;
}
