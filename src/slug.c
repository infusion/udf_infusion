#include "common.h"

DLLEXPORT my_bool slug_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    switch (args->arg_count) {
        case 2:
            args->arg_type[1] = STRING_RESULT;
        case 1:
            args->arg_type[0] = STRING_RESULT;
            break;
        default:
            strcpy(message, "slug must have one or two arguments");
            return 1;
    }

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

    if (args->arg_count == 2 && args->lengths[1] == 1) {
        return _translate_string(args, ptr, length, args->args[1][0]);
    } else {
        return _translate_string(args, ptr, length, '_');
    }
}

DLLEXPORT void slug_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    char *ptr = (char *) initid->ptr;

    if (NULL != ptr) {
        free(ptr);
    }
}
