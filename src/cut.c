#include "common.h"

my_bool cut_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {

    switch (args->arg_count) {
        case 3:
            args->arg_type[2] = STRING_RESULT;
            // no break
        case 2:
            args->arg_type[0] = STRING_RESULT;
            args->arg_type[1] = INT_RESULT;
            break;
        default:
            strcpy(message, "cut must have two or three arguments");
            return 1;
    }

    initid->max_length = args->attribute_lengths[0];
    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->ptr = NULL;

    return 0;
}

char *cut(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *is_null, char *error __attribute__((unused))) {

    char *str = args->args[0], *c = "...";
    long sl = args->lengths[0], cl = 3, max = *((longlong *) args->args[1]), i;

    int space = -1, p = 0, len = 0;


    if (3 == args->arg_count && NULL != args->args[2]) {
        c = args->args[2];
        cl = args->lengths[2];
    }

    if (NULL == str) {
        *is_null = 1;
        return result;
    }

    for (; p < sl; len++) {

        const unsigned char c = str[p];

        switch (c) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                space = p;
        }

        if (len == max) {
            break;
        }

        if ((c & 0x80) == 0)
            p += 1;
        else if ((c & 0xE0) == 0xC0)
            p += 2;
        else if ((c & 0xF0) == 0xE0)
            p += 3;
        else if ((c & 0xF8) == 0xF0)
            p += 4;
        else {
            // Broken UTF8
            *is_null = 1;
            return result;
        }
    }

    // Calculate alloc size
    int tmp_p = p; // Needed to check if resulting string is same as the original => omit ...
    int mem_size = p;
    char *ptr = result;
    if (len == max && tmp_p != sl) {

        if (space != -1) {
            p = mem_size = space;
        }
        mem_size += cl;
    }

    // Alloc new memory if needed
    if (mem_size >= 254) {

        if (NULL == (ptr = malloc(mem_size + 1))) {
            *is_null = 1;
            return result;
        } else {
            initid->ptr = ptr;
        }
    }

    // Copy argument
    memcpy(ptr, str, p);

    // Append dots
    if (len == max && tmp_p != sl) {
        memcpy(ptr + p, c, cl);
    }

    ptr[mem_size] = 0;
    *length = mem_size;

    return ptr;
}

void cut_deinit(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    char *ptr = (char *) initid->ptr;

    if (NULL != ptr) {
        free(ptr);
    }
}
