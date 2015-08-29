#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

#define ARRAY_GET_DOUBLE(a, i) (*((double *) ((char *)(a).p + (a).size*(i))))

struct array {
  void *p;
  size_t n;
  size_t size;
  size_t n_alloc;
};

extern struct array *array_init(struct array *, size_t, size_t);
extern struct array *array_append(struct array *, void *);
extern struct array *array_truncate(struct array *);
extern void array_free(struct array *);

#endif /* ARRAY_H */
