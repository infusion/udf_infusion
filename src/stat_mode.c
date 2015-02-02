#include "common.h"
#include "array.h"


struct Buffer {
    struct array values;
};


my_bool stat_mode_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    struct Buffer *data;

    if (1 != args->arg_count) {
        snprintf(message, MYSQL_ERRMSG_SIZE,
            "stat_mode must have exaclty one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;

    data = calloc(1, sizeof(*data));
    if (NULL == data) {
        snprintf(message, MYSQL_ERRMSG_SIZE, "Memory allocation failed");
        return 1;
    }

    if (NULL == array_init(&data->values, sizeof(double), 32)) {
        snprintf(message, MYSQL_ERRMSG_SIZE, "Memory allocation failed");
        return 1;
    }

    initid->decimals = NOT_FIXED_DEC;
    initid->maybe_null = 1;
    initid->ptr = (char*) data;

    return 0;
}

void stat_mode_clear(UDF_INIT* initid, char* is_null, char *error)
{
    struct Buffer *data = (struct Buffer *) initid->ptr;
    array_truncate(&data->values);
}

void stat_mode_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (NULL == args->args[0])
        return;

    if (NULL == array_append(&data->values, args->args[0])) {
        *error = 1;
        return;
    }
}

void stat_mode_deinit(UDF_INIT *initid)
{
    struct Buffer *data = (struct Buffer *) initid->ptr;
    if (NULL != data) {
        array_free(&data->values);
        free(data);
        data = NULL;
    }
}

static int compar(const void *pa, const void *pb) {
    double a = *((double *) pa);
    double b = *((double *) pb);
    return a > b ? 1 : (a < b ? -1 : 0);
}

double stat_mode(UDF_INIT *initid, UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (data->values.n == 0) {
        *is_null = 1;
        return 0;
    }

    qsort(data->values.p, data->values.n, sizeof(double), compar);

    double mode = 0;
    double value = 0;
    size_t n = 0;
    size_t k = 0;
    size_t i = 0;

    for (i = 0; i < data->values.n; i++) {
        if (ARRAY_GET_DOUBLE(data->values, i) != value) {
            value = ARRAY_GET_DOUBLE(data->values, i);
            k = 0;
        }
        k++;
        if (k > n) {
            mode = value;
            n = k;
        }
    }

    if (n == 0) {
        *is_null = 1;
        return 0;
    }

    return mode;
}
