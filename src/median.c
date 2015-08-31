#include "common.h"
#include "array.h"

struct Buffer {
    struct array values;
};

DLLEXPORT my_bool median_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct Buffer *data;

    if (1 != args->arg_count) {
        snprintf(message, MYSQL_ERRMSG_SIZE,
                "median must have exactly one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;

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

void median_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;
    array_truncate(&data->values);
}

void median_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (NULL == args->args[0])
        return;

    if (NULL == array_append(&data->values, args->args[0])) {
        *error = 1;
        return;
    }
}

DLLEXPORT void median_deinit(UDF_INIT *initid) {
    struct Buffer *data = (struct Buffer *) initid->ptr;
    if (NULL != data) {
        array_free(&data->values);
        free(data);
        data = NULL;
    }
}

DLLEXPORT double median(UDF_INIT *initid, UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (data->values.n == 0) {
        *is_null = 1;
        return 0;
    }

    if (data->values.n % 2 == 0) {
        return _quantile(data->values.p, data->values.n, 0.5);
    } else {
        size_t k = (data->values.n - 1) / 2;
        return _quantile_disc(data->values.p, data->values.n, k);
    }
}
