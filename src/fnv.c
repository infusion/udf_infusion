#include "common.h"

my_bool fnv_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (1 != args->arg_count) {
        strcpy(message, "fnv must have one argument");
        return 1;
    }

    args->arg_type[0] = STRING_RESULT;

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

longlong fnv(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
        char *is_null,
        char *error __attribute__((unused))) {
    char *str = args->args[0];
    size_t len = args->lengths[0];

    const unsigned char *first = (unsigned char *) str, *last = first + len;

    longlong hash = 0xcbf29ce484222325ULL,
            magic = 0x00000100000001b3ULL;

    if (NULL == args->args[0]) {
        *is_null = 1;
        return 0;
    }

    while (first < last) {
        hash ^= *first++;
        hash *= magic;
    }
    return hash;
}
