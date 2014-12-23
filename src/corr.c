#include "common.h"


struct CorrBuffer {
    size_t n;
    double x;
    double y;
    double xy;
    double xx;
    double yy;
};


my_bool corr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    struct CorrBuffer *data;

    if (2 != args->arg_count) {
        strcpy(message, "corr must have exaclty two arguments");
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

void corr_clear(UDF_INIT* initid, char* is_null, char *error)
{
    struct CorrBuffer *data = (struct CorrBuffer *) initid->ptr;

    data->n = 0;
    data->x = 0;
    data->y = 0;
    data->xy = 0;
    data->xx = 0;
    data->yy = 0;
}

void corr_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
    struct CorrBuffer *data = (struct CorrBuffer *) initid->ptr;

    if (NULL == args->args[0] || NULL == args->args[1])
        return;

    double x = *((double *) args->args[0]);
    double y = *((double *) args->args[1]);

    data->n++;
    data->x += x;
    data->y += y;
    data->xy += x*y;
    data->xx += x*x;
    data->yy += y*y;
}

void corr_deinit(UDF_INIT *initid)
{
    struct CorrBuffer *data = (struct CorrBuffer *) initid->ptr;

    if (data) {
        free(data);
    }
}

double corr(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused)))
{
    struct CorrBuffer *data = (struct CorrBuffer *) initid->ptr;

    if (data->n == 0 || data->xx == 0 || data->yy == 0) {
        *is_null = 1;
        return 0;
    }

    /*
     * Corr(X, Y) = Cov(X, Y)/sqrt(D(x)D(Y))
     * Cov(X, Y) = E((X - E(X))(Y - E(Y))) = E(XY) - E(X)E(Y))
     * D(X) = Cov(X, X) = E(XX) - E(X)^2
     */
    double n_inv = 1./data->n;
    double cov_xy = n_inv*(data->xy - n_inv*data->x*data->y);
    double dx = n_inv*(data->xx - n_inv*data->x*data->x);
    double dy = n_inv*(data->yy - n_inv*data->y*data->y);
    return cov_xy/sqrt(dx*dy);
}
