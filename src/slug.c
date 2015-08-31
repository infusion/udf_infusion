#include "common.h"

DLLEXPORT my_bool slug_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    if (1 != args->arg_count) {
        strcpy(message, "slug must have exaclty one argument");
        return 1;
    }

    args->arg_type[0] = STRING_RESULT;

    initid->max_length = args->lengths[0] * 2;
    initid->ptr = NULL;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

DLLEXPORT char *slug(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused))) {
    char *ptr;

    if (NULL == args->args[0]) {
        *is_null = 1;
        return result;
    }

    if (initid->max_length <= 255) {
        ptr = result;
    } else if (NULL != initid->ptr) {
        ptr = initid->ptr;
    } else if (NULL == (ptr = malloc(initid->max_length))) {
        *is_null = 1;
        return result;
    } else {
        initid->ptr = ptr;
    }
    return _translate_string(args, ptr, length, '_');
}

DLLEXPORT void slug_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    char *ptr = (char *) initid->ptr;

    if (NULL != ptr) {
        free(ptr);
    }
}
