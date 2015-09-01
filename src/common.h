#ifndef COMMON_H
#define COMMON_H

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#define __attribute__(x) 
#ifndef __cplusplus
#define inline __inline
#endif
#define snprintf _snprintf

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
http://pubs.opengroup.org/onlinepubs/9699919799/functions/strndup.html
*/
inline static char * strndup(const char *s, size_t n)
{
	char *result;
	size_t len;

	len = strlen(s);
	if (n < len) {
		len = n;
	}
#ifdef __cplusplus
	result = static_cast<char *>(malloc(len + 1));
#else
	result = malloc(len + 1);
#endif
	if (!result) {
		return 0;
	}
	memcpy(result, s, len);
	result[len] = '\0';
	return result;
}
#else /* _MSC_VER */
#define DLLEXPORT 
#endif /* _MSC_VER */

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#if defined(__WIN__) || defined(_WIN32)
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /* defined(__WIN__) || defined(_WIN32) */
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>       /* To get strmov() */
#else
/* when compiled as standalone */
#include <string.h>
#define strmov(a,b) stpcpy(a,b)
#define bzero(a,b) memset(a,0,b)
#define memcpy_fixed(a,b,c) memcpy(a,b,c)
#endif
#endif
#include <mysql.h>
#include <ctype.h>

#ifndef NOT_FIXED_DEC
#define NOT_FIXED_DEC 31
#endif

#ifdef __cplusplus
extern "C" {
#endif

  struct DoubleBuffer {
    unsigned long used;
    unsigned long size;
    double *number;
    double sum;
  };

  struct StatBuffer {
    unsigned long count;
    double mean;
    double M2;
    double M3;
    double M4;
  };

  extern char *_translate_string(UDF_ARGS *, char *, unsigned long *, char);
  extern double _quantile(double *, size_t, double);
  extern double _quantile_disc(double *, size_t, size_t);


#define LESSSIZE()                          \
    if (data->size == data->used) {         \
        data->size<<= 1;                    \
        data->number = (double *) realloc(data->number, data->size * sizeof(*(data->number))); \
        if (NULL == data->number) {         \
            *error = 1;                     \
            return;                         \
        }                                   \
    }

#define LESSINIT()                          \
                                            \
    if (NULL == (data = malloc(sizeof (*data)))) {  \
        strcpy(message, "Memory allocation failed"); \
        return 1;                           \
    }                                       \
    data->used = 0;                         \
    data->sum = 0;                          \
    data->size = 32;                        \
    data->number = (double *) malloc(data->size * sizeof(*(data->number))); \
    if (NULL == data->number) {             \
        strcpy(message, "Memory allocation failed"); \
        return 1;                           \
    }                                       \
                                            \
    initid->maybe_null = 1;                 \
    initid->ptr = (char*) data

#define LESSCLEAR()                         \
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
                                            \
    data->used = 0


#define LESSADD()                           \
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
    double value = *((double*)args->args[0]); \
                                            \
    unsigned int s = doubleSlot(data, value, 0, data->used - 1); \
                                            \
    if (NULL == args->args[0])              \
        return;                             \
                                            \
    LESSSIZE();                             \
                                            \
    switch (type) {                         \
    case 1:                                 \
        data->sum = data->sum + value;      \
        break;                              \
    }                                       \
                                            \
    doublePush(data, s, value);             \
    data->used++


#define LESSDEINIT()                        \
    struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
                                            \
    if (NULL != data->number) {             \
        free(data->number);                 \
    }                                       \
                                            \
    if (NULL != data) {                     \
        free(data);                         \
    }


#define LESS()                              \
    do {                                    \
        psum+= data->number[count];         \
                                            \
        if (psum < limit) {                 \
            count++;                        \
                                            \
            if (count >= data->used)        \
                break;                      \
                                            \
            continue;                       \
        }                                   \
        break;                              \
                                            \
    } while (1)

  inline static unsigned int doubleSlot(struct DoubleBuffer *buffer, double value, int m, int n) {

    double *data = buffer->number;

    unsigned int i;

    if (0 == buffer->used) {
      return 0;
    }

    // TODO: Use binary search

    if (value >= data[buffer->used - 1]) {
      return buffer->used;
    }

    if (value <= data[0]) {
      return 0;
    }

    for (i = 0; i < buffer->used - 1; i++) {

      if (data[i] <= value && value <= data[1 + i]) {
        return i + 1;
      }
    }
    return buffer->used;
  }

  inline static void doublePush(struct DoubleBuffer *buffer, unsigned int step, double value) {

    double *data = buffer->number;

    /* About zero length of memmove()
    7.21.1.2
    "Where an argument declared as size_t n specifies the length of the array
    for a function, n can have the value zero on a call to that function.
    Unless explicitly stated otherwise in the description of a particular
    function in this subclause, pointer arguments on such a call shall still
    have valid values, as described in 7.1.4."
     */
    memmove(&data[1 + step], &data[step], sizeof (*data) * (buffer->used - step));

    data[step] = value;
  }

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
