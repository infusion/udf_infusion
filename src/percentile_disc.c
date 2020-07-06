#include "common.h"
#include "array.h"

struct Buffer {
    struct array values;
    double percentile;
};

DLLEXPORT my_bool percentile_discr_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct Buffer *data;

    if (2 != args->arg_count) {
        snprintf(message, MYSQL_ERRMSG_SIZE,
                "percentile_discr must have exactly two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;

    data = calloc(1, sizeof (*data));
    if (NULL == data) {
        snprintf(message, MYSQL_ERRMSG_SIZE, "Memory allocation failed");
        return 1;
    }

    if (NULL == array_init(&data->values, sizeof (double), 32)) {
        snprintf(message, MYSQL_ERRMSG_SIZE, "Memory allocation failed");
        return 1;
    }

    initid->decimals = NOT_FIXED_DEC;
    initid->maybe_null = 1;
    initid->ptr = (char*) data;

    return 0;
}

DLLEXPORT void percentile_discr_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;
    array_truncate(&data->values);
}

DLLEXPORT void percentile_discr_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    double percentile = *((double *) args->args[1]);
    if (!(isfinite(percentile) && percentile >= 0 && percentile <= 1)) {
        *error = 1;
        return;
    }
    data->percentile = percentile;

    if (NULL == args->args[0])
        return;

    if (NULL == array_append(&data->values, args->args[0])) {
        *error = 1;
        return;
    }
}

DLLEXPORT void percentile_discr_deinit(UDF_INIT *initid) {
    struct Buffer *data = (struct Buffer *) initid->ptr;
    if (NULL != data) {
        array_free(&data->values);
        free(data);
        data = NULL;
    }
}

DLLEXPORT double percentile_discr(UDF_INIT *initid, UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (data->values.n == 0) {
        *is_null = 1;
        return 0;
    }

    size_t k = fmax(0, ceil(data->values.n * data->percentile) - 1);
    return _quantile_disc(data->values.p, data->values.n, k);
}
