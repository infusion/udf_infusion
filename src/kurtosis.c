#include "common.h"

my_bool kurtosis_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    struct StatBuffer *data;

    if (1 != args->arg_count) {
        strcpy(message, "kurtosis must have exaclty one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;

    if (NULL == (data = malloc(sizeof (*data)))) {
        strcpy(message, "Memory allocation failed");
        return 1;
    }

    initid->decimals = NOT_FIXED_DEC;
    initid->maybe_null = 1;
    initid->ptr = (char*) data;

    return 0;
}

void kurtosis_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

    data->count = 0;
    data->mean = 0;
    data->M2 = 0;
    data->M3 = 0;
    data->M4 = 0;
}

void kurtosis_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

    if (NULL == args->args[0])
        return;

    // http://people.xiph.org/~tterribe/notes/homs.html

    double value = *((double *) args->args[0]);

    data->count++;

    double n = data->count;

    double delta = value - data->mean;

    double delta_n = delta / n;
    double shorten = delta * delta_n * (n - 1);

    data->mean = data->mean + delta_n;
    data->M4 = data->M4 + delta_n * (shorten * delta_n * (3 + n * (n - 3)) + 6 * delta_n * data->M2 - 4 * data->M3);
    data->M3 = data->M3 + shorten * delta_n * (n - 2) - 3 * delta_n * data->M2;
    data->M2 = data->M2 + shorten;
}

void kurtosis_deinit(UDF_INIT *initid) {
    struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

    if (data) {
        free(data);
    }
}

double kurtosis(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {

    struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

    if (data->count < 2) {
        *is_null = 1;
        return 0;
    }

    return data->count * data->M4 / (data->M2 * data->M2) - 3;
}
