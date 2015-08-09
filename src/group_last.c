#include "common.h"

struct Buffer {
    longlong length;
    char *string;
    char state;
};

my_bool group_last_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct Buffer* data;

    if (1 != args->arg_count) {
        strcpy(message, "group_last must have exaclty one argument");
        return 1;
    }

    args->arg_type[0] = STRING_RESULT;

    if (NULL == (data = malloc(sizeof (*data)))) {
        strcpy(message, "Memory allocation failed");
        return 1;
    }

    if (NULL == (data->string = malloc(65535))) {
        strcpy(message, "Memory allocation failed");
        free(data);
        return 1;
    }
    data->length = 0;
    data->state = 0;

    initid->max_length = 65535;
    initid->maybe_null = 1;
    initid->ptr = (char*) data;

    return 0;
}

void group_last_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct Buffer* data = (struct Buffer *) initid->ptr;

    data->length = 0;
    data->state = 0;
}

void group_last_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (NULL != args->args[0]) {
        memcpy(data->string, args->args[0], args->lengths[0]);
        data->length = args->lengths[0];
        data->state = 1;
    } else if (data->state == 0) {
        data->state = 2;
    }
}

void group_last_deinit(UDF_INIT *initid) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (data) {

        if (data->string) {
            free(data->string);
        }
        free(data);
    }
}

char *group_last(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused))) {
    struct Buffer* data = (struct Buffer *) initid->ptr;

    if (data->state == 2 || data->string == NULL) {
        *is_null = 1;
        (*length) = 0;
        return NULL;
    }

    initid->max_length = data->length;
    result = data->string;

    (*length) = data->length;

    return result;
}
