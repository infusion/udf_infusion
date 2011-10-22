/* Copyright (C) 2010 Robert Eisele <robert@xarg.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>		/* To get strmov() */
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

#ifndef HAVE_DLOPENxxx

struct Buffer {
	long long length;
	char *string;
	char state;
};

struct DoubleBuffer {
	unsigned long used;
	unsigned long size;
	double *number;
	double sum;
};


static char *_translate_string(UDF_ARGS *args, char *result, unsigned long *length, char separator);

my_bool isbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "isbit must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong isbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{

	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong bit = *((longlong *) args->args[0]);
	longlong n = *((longlong *) args->args[1]);

	return (bit >> n) & 1;
}

my_bool setbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	switch (args->arg_count) {
	case 3:
		args->arg_type[2] = INT_RESULT;
	case 2:
		args->arg_type[0] = INT_RESULT;
		args->arg_type[1] = INT_RESULT;
		break;
	default:
		strcpy(message, "setbit must have two or three arguments");
		return 1;
	}

	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong setbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong bit = *((longlong *) args->args[0]);
	longlong n = *((longlong *) args->args[1]);

	if (2 == args->arg_count || NULL != args->args[2] && 1 == *((longlong *) args->args[2])) {
		return bit | (1 << n);
	}
	return bit & (~(1 << n));
}

my_bool invbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "invbit must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong invbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong bit = *((longlong *) args->args[0]);
	longlong n = *((longlong *) args->args[1]);

	return bit ^ (1 << n);
}

my_bool rotbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "rotbit must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong rotbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong bit = *((longlong *) args->args[0]);
	longlong n = *((longlong *) args->args[1]);

	n = (64 + (n % 64)) % 64;
	return ((bit << n) | (bit >> (64 - n))) & 0x7FFFFFFFFFFFFFFFLL;
}

my_bool numbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (1 != args->arg_count && 2 != args->arg_count) {
		strcpy(message, "numbit must have one or two one arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 0;

	return 0;
}

longlong numbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0]) {
		*is_null = 1;
		return 0;
	}

	short c;
	longlong bit = *((longlong *) args->args[0]);

	if (NULL != args->args[1] && 0 == *((longlong *) args->args[1])) {
		bit = ~bit;
	}

	for (c = 0; bit; bit &= bit - 1, c++);

	return c;
}

my_bool msbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (1 != args->arg_count) {
		strcpy(message, "msbit must have exactly one single argument");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong msbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0]) {
		*is_null = 1;
		return 0;
	}
	ulonglong v = *((longlong *) args->args[0]);
	unsigned r = 0;

	while (v >>= 1) {
		r++;
	}
	return r;
}

my_bool getint_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (3 != args->arg_count) {
		strcpy(message, "getint must have exactly three arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	args->arg_type[2] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong getint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2]) {
		*is_null = 1;
		return 0;
	}
	longlong n = *((longlong *) args->args[0]);
	longlong x = *((longlong *) args->args[1]);
	longlong y = *((longlong *) args->args[2]);

	return (n >> x) & ((2 << (y - x)) - 1);
}

my_bool rotint_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (4 != args->arg_count) {
		strcpy(message, "rotint must have exactly four arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	args->arg_type[2] = INT_RESULT;
	args->arg_type[3] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong rotint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	longlong n, x, y, m, a, b;

	if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2] || NULL == args->args[3]) {
		*is_null = 1;
		return 0;
	}

	n = *((longlong *) args->args[0]);
	x = *((longlong *) args->args[1]);
	y = *((longlong *) args->args[2]);
	m = *((longlong *) args->args[3]);

	if (y < x || x < 1) {
		*is_null = 1;
		return 0;
	}

	y-= x;
	m = (y + (m % y)) % y;

	a = (1 << y) - 1;
	b = (n >> x) & a;

	return (n & (~(a << x))) | ((((b << m) | (b >> (y - m))) & a) << x);
}

my_bool setint_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (4 != args->arg_count) {
		strcpy(message, "setint must have exactly four arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	args->arg_type[2] = INT_RESULT;
	args->arg_type[3] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

longlong setint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2] || NULL == args->args[3]) {
		*is_null = 1;
		return 0;
	}
	longlong n = *((longlong *) args->args[0]);
	longlong x = *((longlong *) args->args[1]);
	longlong y = *((longlong *) args->args[2]);
	longlong m = *((longlong *) args->args[3]);

	return n & (~(((2 << (y - x)) - 1) << x)) | (m << x);
}

my_bool bround_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "bround must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;
	args->arg_type[1] = REAL_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

double bround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1] || 0 == *((double *) args->args[1])) {
		*is_null = 1;
		return 0;
	}

	return ceil(*((double *) args->args[0]) / *((double *) args->args[1])) * *((double *) args->args[1]);
}

my_bool xround_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (1 != args->arg_count) {
		strcpy(message, "xround must have exactly one single argument");
		return 1;
	}

	switch (args->arg_type[0]) {
	case STRING_RESULT:
	case DECIMAL_RESULT:
		args->arg_type[0] = REAL_RESULT;
		break;
	}
	initid->const_item = 1;
	initid->maybe_null = 0;

	return 0;
}

longlong xround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	longlong n, x = 1LL;
	double d;

	if (NULL == args->args[0]) {
		*is_null = 1;
		return 0;
	}

	if (INT_RESULT == args->arg_type[0]) {

		n = *((longlong *) args->args[0]);

	} else {

		d = *((double *) args->args[0]);
		n = (longlong) d;

		n = n + (n < d) - (d < n);
	}

	if (n < 0) {
		n = -n;
		x = -1;
	}

	if (n > 1000000000LL) {

		if (n > 100000000000000LL) {
			if (n > 10000000000000000LL) {
				if (n <= 100000000000000000LL) return x * 100000000000000000LL;
				if (n <= 1000000000000000000LL) return x * 1000000000000000000LL;
				return x * 1000000000000000000LL;
			} else {
				if (n <= 1000000000000000LL) return x * 1000000000000000LL;
				return x * 10000000000000000LL;
			}
		} else {
			if (n > 100000000000LL) {
				if (n <= 1000000000000LL) return x * 1000000000000LL;
				if (n <= 10000000000000LL) return x * 10000000000000LL;
				return x * 100000000000000LL;
			} else {
				if (n <= 10000000000LL) return x * 10000000000LL;
				return x * 100000000000LL;
			}
		}
	} else if (n > 10000LL) {
		if (n > 1000000LL) {
			if (n <= 10000000LL) return x * 10000000LL;
			if (n <= 100000000LL) return x * 100000000LL;
			return x * 1000000000LL;
		} else {
			if (n <= 100000LL) return x * 100000LL;
			return x * 1000000LL;
		}
	} else {
		if (n > 100LL) {
			if (n <= 1000LL) return x * 1000LL;
			return x * 10000LL;
		} else {
			if (n <= 1LL) return x;
			if (n <= 10LL) return x * 10LL;
			return x * 100LL;
		}
	}
	return x;
}

my_bool thumbscale_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (3 != args->arg_count) {
		strcpy(message, "thumbscale must have exactly three arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	args->arg_type[2] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

longlong thumbscale(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2]) {
		*is_null = 1;
		return 0;
	}

	longlong up = *((longlong *) args->args[0]);
	longlong down = *((longlong *) args->args[1]);
	longlong scale = *((longlong *) args->args[2]);

	return round(scale * up / (double) (down + up));
}

my_bool thumbratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "thumbratio must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

double thumbratio(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong up = *((longlong *) args->args[0]);
	longlong down = *((longlong *) args->args[1]);

	return (double) (up + down) / (1 + abs(up - down));
}

my_bool starratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count < 2) {
		strcpy(message, "starratio must have at least two arguments");
		return 1;
	}

	size_t i = args->arg_count;
	while (i--) {
		args->arg_type[i] = INT_RESULT;
	}

	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

double starratio(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	size_t i = args->arg_count;
	longlong n = 0, d = 0;

	while (i--) {

		if (NULL == args->args[i]) {
			*is_null = 1;
			return 0;
		}
		n += *((longlong *) args->args[i]) * ((longlong) i + 1);
		d += *((longlong *) args->args[i]);
	}

	if (!d) {
		return 0;
	}

	return n / (double) d;
}

my_bool bound_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (3 != args->arg_count) {
		strcpy(message, "bound must have exactly three arguments");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;
	args->arg_type[1] = REAL_RESULT;
	args->arg_type[2] = REAL_RESULT;

	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

double bound(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	double *n, *x, *y;

	n = (double *) args->args[0];
	x = (double *) args->args[1];
	y = (double *) args->args[2];

	if (NULL == n) {
		*is_null = 1;
		return 0;
	}

	if (NULL != y && NULL != x && *y < *x) {
		return *x;
	}

	if (NULL != y && *y < *n) {
		return *y;
	}

	if (NULL != x && *n < *x) {
		return *x;
	}
	return *n;
}

my_bool cut_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count && 3 != args->arg_count) {
		strcpy(message, "cut must have two or three arguments");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = INT_RESULT;
	args->arg_type[2] = STRING_RESULT;

	initid->max_length = args->attribute_lengths[0];
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

char *cut(UDF_INIT *initid, UDF_ARGS *args,
		char *result, unsigned long *length,
		char *is_null, char *error __attribute__((unused)))
{

	char *str = args->args[0], *c = "...";
	long sl = args->lengths[0], cl = 3, max = *((longlong *) args->args[1]), i;

	if (NULL == str) {
		*is_null = 1;
		return 0;
	}

	if (3 == args->arg_count && NULL != args->args[2]) {
		c = args->args[2];
		cl = args->lengths[2];
	}

	if (max > 0 && (sl - cl) > max) {

		for (i = (sl - 1) < max ? (sl - 1) : max; i >= 0; i--) {
			switch (str[i]) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				goto done;
			}
		}
done:
		if (-1 == i) {
			*length = sl < max ? sl : max;
		} else {
			*length = i;
		}

		memcpy(result, str, *length);
		memcpy(result + *length, c, cl);
		*length+= cl;
		return result;
	}

	memcpy(result, str, sl);
	*length = sl;
	result[*length] = 0;
	return result;
}

my_bool slug_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (1 != args->arg_count) {
		strcpy(message, "slug must have exaclty one argument");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	initid->max_length = args->attribute_lengths[0] * 2;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

char *slug(UDF_INIT *initid, UDF_ARGS *args,
		char *result, unsigned long *length,
		char *is_null, char *error __attribute__((unused)))
{
	if (NULL == args->args[0]) {
		*is_null = 1;
		return 0;
	}

	return _translate_string(args, result, length, '-');
}

my_bool ngram_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
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

	initid->max_length = args->lengths[0] * 11;
	initid->const_item = 1;
	initid->maybe_null = 1;

	return 0;
}

char *ngram(UDF_INIT *initid, UDF_ARGS *args,
		char *result, unsigned long *length,
		char *is_null, char *error __attribute__((unused)))
{
	char *tmp, *start_res = result;

	long i = -1, j, l, n = 2;

	if (NULL == args->args[0]) {
		*is_null = 1;
		return 0;
	}

	if (2 == args->arg_count) {
		if ((n = (unsigned) *((long long *) args->args[1])) > 10) n = 2;
	}

	if (n < 1 || args->lengths[0] < 1) {
		*length = 0;
		return result;
	}

	result = _translate_string(args, result, length, '_');

	l = *length;

	if (l < 1 || l < n) {
		return result;
	}

	tmp = strndup(result, l);

	for (result = start_res; i < l; i++) {
		if (i < l - n + 2) {
			for (j = 0; j < n; j++) {
				if (i + j >= 0 && i + j < l) {
					*(result++) = tmp[i + j];
				} else {
					*(result++) = '_';
				}
			}
			*(result++) = ' ';
		}
	}
	free(tmp);

	*(--result) = 0;

	*length = result - start_res;

	return start_res;
}

#if 1
#  define EN(n)		(n)
#else
#  define EN(n)		(((n) >> 8) | (((n) << 8) & 0xffff))
#endif

#define ADD1(s)		*(result++) = s[0]; add = 1;
#define ADD1I(s)	ADD1(s); ++ptr;

#define ADD2(s)		*(result++) = s[0]; *(result++) = s[1]; add = 1;
#define ADD2I(s)	ADD2(s); ++ptr;

#define BTW(c, a, b)	(unsigned)(c) - (unsigned)(a) <= (unsigned)(b) - (unsigned)(a)

static char *_translate_string(UDF_ARGS *args, char *result, unsigned long *length, char separator)
{
	char *start_res = result, *ptr = args->args[0], *end_str = ptr + args->lengths[0], add = 0;

	/* My own UTF8 and latin translation table */

	for (; ptr < end_str; ptr++) {

		unsigned char c = *ptr;
		unsigned short s = *((unsigned short *) ptr);

		if (BTW(c, 'a', 'z') || BTW(c, '0', '9')) {
			*(result++) = c;		add = 1;
		} else if (BTW(c, 'A', 'Z')) {
			*(result++) = c | 32;	add = 1;
		} else switch (s) {
		case EN(0x9fc3):
			ADD2I("ss");
			break;
		case EN(0x84c3): case EN(0xa4c3):
		case EN(0xa6c3): case EN(0x86c3):
			ADD2I("ae");
			break;
		case EN(0x96c3): case EN(0xb6c3):
			ADD2I("oe");
			break;
		case EN(0x9cc3): case EN(0xbcc3):
			ADD2I("ue");
			break;
		case EN(0xa0c3): case EN(0xa1c3):
		case EN(0xa2c3): case EN(0xa3c3):
		case EN(0xa5c3): case EN(0x80c3):
		case EN(0x81c3): case EN(0x82c3):
		case EN(0x83c3): case EN(0x85c3):
			ADD1I("a");
			break;
		case EN(0xa8c3): case EN(0xa9c3):
		case EN(0xaac3): case EN(0xabc3):
		case EN(0x88c3): case EN(0x89c3):
		case EN(0x8ac3): case EN(0x8bc3):
			ADD1I("e");
			break;
		case EN(0xacc3): case EN(0xadc3):
		case EN(0xaec3): case EN(0xafc3):
		case EN(0x8cc3): case EN(0x8dc3):
		case EN(0x8ec3): case EN(0x8fc3):
			ADD1I("i");
			break;
		case EN(0xb0c3): case EN(0xb2c3):
		case EN(0xb3c3): case EN(0xb4c3):
		case EN(0xb5c3): case EN(0x92c3):
		case EN(0x93c3): case EN(0x94c3):
		case EN(0x95c3):
			ADD1I("o");
			break;
		case EN(0xb9c3): case EN(0xbac3):
		case EN(0xbbc3): case EN(0x99c3):
		case EN(0x9ac3): case EN(0x9bc3):
		case EN(0xb5c2):
			ADD1I("u");
			break;
		case EN(0x91c3): case EN(0xb1c3):
			ADD1I("n");
			break;
		case EN(0x87c3): case EN(0xa7c2):
		case EN(0xa2c2): case EN(0xa7c3):
			ADD1I("c");
			break;
		case EN(0x9ec3): case EN(0xbec3):
			ADD1I("p");
			break;
		case EN(0xbfc3): case EN(0xa5c2):
		case EN(0x9dc3): case EN(0xbdc3):
			ADD1I("y");
			break;
		case EN(0x97c3):
			ADD1I("x");
			break;
		case EN(0xaec2):
			ADD1I("r");
			break;
		case EN(0x90c3):
			ADD1I("d");
			break;
		default:
			switch (c) {
			case 223:
				ADD2("ss");
				break;
			case 196: case 198:
			case 228: case 230:
				ADD2("ae");
				break;
			case 214: case 246:
				ADD2("oe");
				break;
			case 220: case 252:
				ADD2("ue");
				break;
			case 192: case 193:
			case 194: case 195:
			case 197: case 224:
			case 225: case 226:
			case 227: case 229:
				ADD1("a");
				break;
			case 200: case 201:
			case 202: case 203:
			case 232: case 233:
			case 234: case 235:
				ADD1("e");
				break;
			case 161: case 204:
			case 205: case 206:
			case 207: case 236:
			case 237: case 238:
			case 239:
				ADD1("i");
				break;
			case 210: case 211:
			case 212: case 213:
			case 240: case 242:
			case 243: case 244:
			case 245:
				ADD1("o");
				break;
			case 181: case 217:
			case 218: case 219:
			case 249: case 250:
			case 251:
				ADD1("u");
				break;
			case 209: case 241:
				ADD1("n");
				break;
			case 162: case 169:
			case 199: case 231:
				ADD1("c");
				break;
			case 222: case 254:
				ADD1("p");
				break;
			case 165: case 221:
			case 253: case 255:
				ADD1("y");
				break;
			case 215:
				ADD1("x");
				break;
			case 174:
				ADD1("r");
				break;
			case 208:
				ADD1("d");
				break;
			default:
				if (add) {
					*(result++) = separator;
					add = 0;
				}
			}
		}
	}

	if ((*length = result - start_res) > 0 && *(result - 1) == separator) {
		result--;
		(*length)--;
	}

	*result = 0;

	return start_res;
}
#undef EN
#undef BTW
#undef ADD1
#undef ADD1I
#undef ADD2
#undef ADD2I

my_bool group_first_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct Buffer* data;

	if (1 != args->arg_count) {
		strcpy(message, "group_first must have exaclty one argument");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	if (!(data = malloc(sizeof (*data)))) {
		strcpy(message, "Memory allocation failed");
		return 1;
	}

	if (!(data->string = malloc(65535))) {
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

void group_first_clear(UDF_INIT* initid, char* is_null, char *error)
{
	struct Buffer* data = (struct Buffer *) initid->ptr;

	data->length = 0;
	data->state = 0;
}

void group_first_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
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

void group_first_deinit(UDF_INIT *initid)
{
	struct Buffer *data = (struct Buffer *) initid->ptr;

	if (data) {

		if (data->string) {
			free(data->string);
		}
		free(data);
	}
}

char *group_first(UDF_INIT *initid, UDF_ARGS *args,
		char *result, unsigned long *length,
		char *is_null, char *error __attribute__((unused)))
{
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

my_bool group_last_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct Buffer* data = NULL;

	if (1 != args->arg_count) {
		strcpy(message, "group_last must have exaclty one argument");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	if (!(data = malloc(sizeof (*data)))) {
		strcpy(message, "Memory allocation failed");
		return 1;
	}
	data->length = 0;
	data->string = NULL;

	initid->max_length = 65535;
	initid->maybe_null = 1;
	initid->ptr = (char*) data;

	return 0;
}

void group_last_clear(UDF_INIT* initid, char* is_null, char *error)
{
	struct Buffer* data = (struct Buffer *) initid->ptr;

	data->length = 0;
	data->string = NULL;
}

void group_last_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct Buffer *data = (struct Buffer *) initid->ptr;

	data->string = args->args[0];
	data->length = args->lengths[0];
}

void group_last_deinit(UDF_INIT *initid)
{
	struct Buffer *data = (struct Buffer *) initid->ptr;

	if (NULL != data) {
		free(initid->ptr);
	}
}

char *group_last(UDF_INIT *initid, UDF_ARGS *args,
		char *result, unsigned long *length,
		char *is_null, char *error __attribute__((unused)))
{
	struct Buffer* data = (struct Buffer *) initid->ptr;

	if (data->string == NULL) {
		*is_null = 1;
		(*length) = 0;
		return NULL;
	}

	initid->max_length = data->length;
	result = data->string;

	(*length) = data->length;

	return result;
}

my_bool noverk_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (2 != args->arg_count) {
		strcpy(message, "noverk must have exactly two arguments");
		return 1;
	}

	args->arg_type[0] = INT_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->const_item = 1;
	initid->maybe_null = 1;
	initid->decimals = 5;
	initid->max_length = 20;

	return 0;
}

longlong noverk(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	if (NULL == args->args[0] || NULL == args->args[1]) {
		*is_null = 1;
		return 0;
	}

	longlong i, res = 1;

	longlong n = *((longlong *)args->args[0]);
	longlong k = *((longlong *)args->args[1]);

	if (!(k > 0 && n > 0 && n > k)) {
		*is_null = 1;
		return 0;
	}

	n = n - k;

	for (i = 1; i <= k; i++) {
		res = res * (n + i) / i;
	}

	return res;
}

#endif /* HAVE_DLOPEN */
