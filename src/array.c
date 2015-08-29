#include "array.h"

struct array * array_init(struct array *a, size_t size, size_t n_alloc) {

    a->p = calloc(n_alloc, size);
    if (NULL == a->p) {
        return NULL;
    }
    a->size = size;
    a->n = 0;
    a->n_alloc = n_alloc;
    return a;
}

struct array * array_append(struct array *a, void *value) {

    if (a->n == a->n_alloc) {
        size_t n_alloc = a->n_alloc * 2;
        void *p = realloc(a->p, a->size * n_alloc);
        if (NULL == p) {
            return NULL;
        }
        memset((char *)p + a->size * a->n_alloc, 0, a->size * (n_alloc - a->n_alloc));
        a->p = p;
        a->n_alloc = n_alloc;
    }
    memcpy((char *)a->p + a->size * a->n, value, a->size);
    a->n++;
    return a;
}

struct array *array_truncate(struct array *a) {
    a->n = 0;
    return a;
}

void array_free(struct array *a) {

    if (NULL != a->p) {
        free(a->p);
    }
    a->p = NULL;
    a->n_alloc = 0;
    a->size = 0;
    a->n = 0;
}
