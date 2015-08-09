#include "common.h"


#define MAX_GRAM 10

my_bool ngram_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    switch (args->arg_count) {
        case 2:
            args->arg_type[1] = INT_RESULT;
        case 1:
            args->arg_type[0] = STRING_RESULT;
            break;
        default:
            strcpy(message, "ngram must have one or two arguments");
            return 1;
    }

    initid->max_length = (args->lengths[0] + 1) * MAX_GRAM;
    initid->ptr = NULL;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

char *ngram(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused))) {

    char *tmp, *ptr, *start;

    long i = -1, j, l, n = 2;

    if (NULL == args->args[0]) {
        *is_null = 1;
        return result;
    }

    if (2 == args->arg_count) {
        if ((n = (unsigned) *((longlong *) args->args[1])) > MAX_GRAM)
            n = MAX_GRAM;
    }

    if (args->lengths[0] < 1 || n < 1) {
        *length = 0;
        return result;
    }

    if (initid->max_length * n <= 255 * MAX_GRAM) {
        ptr = result;
    } else if (NULL != initid->ptr) {
        ptr = initid->ptr;
    } else if (NULL == (ptr = malloc(initid->max_length))) {
        *is_null = 1;
        return result;
    } else {
        initid->ptr = ptr;
    }

    start = ptr;

    _translate_string(args, ptr, length, '_');

    l = *length;

    if (l < 1 || l < n) {
        return ptr;
    }

    tmp = strndup(ptr, l);

    for (; i < l; i++) {
        if (i < l - n + 2) {
            for (j = 0; j < n; j++) {
                if (i + j >= 0 && i + j < l) {
                    *(ptr++) = tmp[i + j];
                } else {
                    *(ptr++) = '_';
                }
            }
            *(ptr++) = ' ';
        }
    }
    free(tmp);

    *(--ptr) = 0;

    *length = ptr - start;

    return start;
}

my_bool ngram_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    char *ptr = (char *) initid->ptr;

    if (NULL != ptr) {
        free(ptr);
    }
    return 0;
}
