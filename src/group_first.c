#include "common.h"

struct Buffer {
    longlong length;
    char *string;
    char state;
};

DLLEXPORT my_bool group_first_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    struct Buffer* data;

    if (1 != args->arg_count) {
        strcpy(message, "group_first must have exaclty one argument");
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

DLLEXPORT void group_first_clear(UDF_INIT* initid, char* is_null, char *error) {
    struct Buffer* data = (struct Buffer *) initid->ptr;

    data->length = 0;
    data->state = 0;
}

DLLEXPORT void group_first_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    switch (data->state) {
        case 0:

            if (NULL == args->args[0]) {
                data->state = 2;
            } else {
                memcpy(data->string, args->args[0], args->lengths[0]);
                data->length = args->lengths[0];
                data->state = 1;
            }
            break;
    }
}

DLLEXPORT void group_first_deinit(UDF_INIT *initid) {
    struct Buffer *data = (struct Buffer *) initid->ptr;

    if (data) {

        if (data->string) {
            free(data->string);
        }
        free(data);
    }
}

DLLEXPORT char *group_first(UDF_INIT *initid, UDF_ARGS *args,
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
