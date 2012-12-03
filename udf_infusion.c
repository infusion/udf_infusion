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


#ifndef NOT_FIXED_DEC
#define NOT_FIXED_DEC 31
#endif

#ifndef HAVE_DLOPENxxx

struct Buffer {
	longlong length;
	char *string;
	char state;
};

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

struct CovBuffer {
	double x;
	double y;
	double c;
	unsigned count;
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

	if (2 == args->arg_count || (NULL != args->args[2] && 1 == *((longlong *) args->args[2]))) {
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
	initid->decimals = NOT_FIXED_DEC;
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
	default:
		break;
	}
	initid->const_item = 1;
	initid->maybe_null = 0;

	return 0;
}

#define RET_NEG(s) return (((s) ^ -f) + f)
longlong xround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	longlong n, f = 0;
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
		f = 1;
	}

	if (n > 1000000000LL) {

		if (n > 100000000000000LL) {
			if (n > 10000000000000000LL) {
				if (n <= 100000000000000000LL) RET_NEG(100000000000000000LL);
				if (n <= 1000000000000000000LL) RET_NEG(1000000000000000000LL);
				RET_NEG(1000000000000000000LL);
			} else {
				if (n <= 1000000000000000LL) RET_NEG(1000000000000000LL);
				RET_NEG(10000000000000000LL);
			}
		} else {
			if (n > 100000000000LL) {
				if (n <= 1000000000000LL) RET_NEG(1000000000000LL);
				if (n <= 10000000000000LL) RET_NEG(10000000000000LL);
				RET_NEG(100000000000000LL);
			} else {
				if (n <= 10000000000LL) RET_NEG(10000000000LL);
				RET_NEG(100000000000LL);
			}
		}
	} else if (n > 10000LL) {
		if (n > 1000000LL) {
			if (n <= 10000000LL) RET_NEG(10000000LL);
			if (n <= 100000000LL) RET_NEG(100000000LL);
			RET_NEG(1000000000LL);
		} else {
			if (n <= 100000LL) RET_NEG(100000LL);
			RET_NEG(1000000LL);
		}
	} else {
		if (n > 100LL) {
			if (n <= 1000LL) RET_NEG(1000LL);
			RET_NEG(10000LL);
		} else {
			if (n <= 1LL) RET_NEG(1LL);
			if (n <= 10LL) RET_NEG(10LL);
			RET_NEG(100LL);
		}
	}
	RET_NEG(1LL);
}
#undef RET_NEG

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
	initid->decimals = NOT_FIXED_DEC;
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

my_bool fnv_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
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
		char *error __attribute__((unused)))
{
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
		hash^= *first++,
		hash*= magic;
	}
	return hash;
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
		if ((n = (unsigned) *((longlong *) args->args[1])) > 10) n = 2;
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
			*(result++) = c;
			add = 1;
		} else if (BTW(c, 'A', 'Z')) {
			*(result++) = c | 32;
			add = 1;
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

my_bool row_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	longlong *data;

	if (0 != args->arg_count) {
		strcpy(message, "rownumber doesn't require an argument");
		return 1;
	}

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Couldn't allocate memory");
		return 1;
	}
	*data = 0;

	initid->ptr = (char *) data;
	initid->const_item = 0;
	initid->maybe_null = 1;

	return 0;
}

longlong row_number(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	longlong *data;

	if (initid->ptr) {
		data = (longlong *) initid->ptr;
		return ++(*data);
	}

	*error = 1;
	return 0;
}

void row_number_deinit(UDF_INIT *initid)
{
	if (initid->ptr) {
		free(initid->ptr);
	}
}

my_bool rsumi_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	longlong *data;

	if (1 != args->arg_count) {
		strcpy(message, "rsumi must have one argument");
		return 1;
	}

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Couldn't allocate memory");
		return 1;
	}
	*data = 0;

	args->arg_type[0] = INT_RESULT;
	
	initid->ptr = (char *) data;
	initid->const_item = 0;
	initid->maybe_null = 1;

	return 0;
}

longlong rsumi(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	longlong *data;

	if (initid->ptr) {
		data = (longlong *) initid->ptr;

		if (NULL == args->args[0]) {
			return *data;
		}

		(*data)+= *((longlong *) args->args[0]);

		return *data;
	}
	*error = 1;
	return 0;
}

void rsumi_deinit(UDF_INIT *initid)
{
	if (initid->ptr) {
		free(initid->ptr);
	}
}

my_bool rsumd_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	double *data;

	if (1 != args->arg_count) {
		strcpy(message, "rsumd must have one argument");
		return 1;
	}

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Couldn't allocate memory");
		return 1;
	}
	*data = 0;

	args->arg_type[0] = REAL_RESULT;
	
	initid->ptr = (char *) data;
	initid->const_item = 0;
	initid->maybe_null = 1;

	return 0;
}

double rsumd(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	double *data;

	if (initid->ptr) {
		data = (double *) initid->ptr;

		if (NULL == args->args[0]) {
			return *data;
		}

		(*data)+= *((double *) args->args[0]);

		return *data;
	}
	*error = 1;
	return 0;
}

void rsumd_deinit(UDF_INIT *initid)
{
	if (initid->ptr) {
		free(initid->ptr);
	}
}

my_bool group_first_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
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

	if (NULL == (data = malloc(sizeof (*data)))) {
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
	initid->decimals = NOT_FIXED_DEC;
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

	longlong n = *((longlong *) args->args[0]);
	longlong k = *((longlong *) args->args[1]);

	if (!(k > 0 && n > 0 && n > k)) {
		*is_null = 1;
		return 0;
	}

	n = n - k;

	for (i = 1; i <= k; i++) {
		res = res * (n + i) / i; // Simplified from res = res * (n - i + 1) / i
	}

	return res;
}

my_bool skewness_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct StatBuffer *data;

	if (1 != args->arg_count) {
		strcpy(message, "skewness must have exaclty one argument");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Memory allocation failed");
		return 1;
	}

	initid->decimals = NOT_FIXED_DEC;
	initid->maybe_null = 1;
	initid->ptr = (char*) data;

	return 0;
}

void skewness_clear(UDF_INIT* initid, char* is_null, char *error)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	data->count = 0;
	data->mean = 0;
	data->M2 = 0;
	data->M3 = 0;
}

void skewness_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (NULL == args->args[0])
		return;

	// http://people.xiph.org/~tterribe/notes/homs.html

	double value = *((double *) args->args[0]);

	data->count++;

	double n = data->count;

	double delta = value - data->mean;

	double delta_n = delta / n;
	double shorten = delta * delta_n * (n - 1);

	data->mean = data->mean + delta_n;
	data->M3 = data->M3 + shorten * delta_n * (n - 2) - 3 * delta_n * data->M2;
	data->M2 = data->M2 + shorten;
}

void skewness_deinit(UDF_INIT *initid)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (data) {
		free(data);
	}
}

double skewness(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{

	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (data->count < 2) {
		*is_null = 1;
		return 0;
	}

	return pow(data->count, 0.5) * data->M3 / pow(data->M2, 1.5);
}

my_bool kurtosis_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct StatBuffer *data;

	if (1 != args->arg_count) {
		strcpy(message, "kurtosis must have exaclty one argument");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Memory allocation failed");
		return 1;
	}

	initid->decimals = NOT_FIXED_DEC;
	initid->maybe_null = 1;
	initid->ptr = (char*) data;

	return 0;
}

void kurtosis_clear(UDF_INIT* initid, char* is_null, char *error)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	data->count = 0;
	data->mean = 0;
	data->M2 = 0;
	data->M3 = 0;
	data->M4 = 0;
}

void kurtosis_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (NULL == args->args[0])
		return;

	// http://people.xiph.org/~tterribe/notes/homs.html

	double value = *((double *) args->args[0]);

	data->count++;

	double n = data->count;

	double delta = value - data->mean;

	double delta_n = delta / n;
	double shorten = delta * delta_n * (n - 1);

	data->mean = data->mean + delta_n;
	data->M4 = data->M4 + delta_n * (shorten * delta_n * (3 + n * (n - 3)) + 6 * delta_n * data->M2 - 4 * data->M3);
	data->M3 = data->M3 + shorten * delta_n * (n - 2) - 3 * delta_n * data->M2;
	data->M2 = data->M2 + shorten;
}

void kurtosis_deinit(UDF_INIT *initid)
{
	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (data) {
		free(data);
	}
}

double kurtosis(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{

	struct StatBuffer *data = (struct StatBuffer *) initid->ptr;

	if (data->count < 2) {
		*is_null = 1;
		return 0;
	}

	return data->count * data->M4 / (data->M2 * data->M2) - 3;
}

my_bool covariance_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct CovBuffer *data;

	if (2 != args->arg_count) {
		strcpy(message, "covariance must have exaclty two arguments");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;
	args->arg_type[1] = REAL_RESULT;

	if (NULL == (data = malloc(sizeof (*data)))) {
		strcpy(message, "Memory allocation failed");
		return 1;
	}

	initid->decimals = NOT_FIXED_DEC;
	initid->maybe_null = 1;
	initid->ptr = (char*) data;

	return 0;
}

void covariance_clear(UDF_INIT* initid, char* is_null, char *error)
{
	struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

	data->count = 0;
	data->x = 0;
	data->y = 0;
	data->c = 0;
}

void covariance_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

	if (NULL == args->args[0] || NULL == args->args[1])
		return;

	double x = *((double *) args->args[0]);
	double y = *((double *) args->args[1]);

	data->count++;

	data->x+= x;
	data->y+= y;
	data->c+= x * y;
}

void covariance_deinit(UDF_INIT *initid)
{
	struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

	if (data) {
		free(data);
	}
}

double covariance(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
		char *is_null,
		char *error __attribute__((unused)))
{
	struct CovBuffer *data = (struct CovBuffer *) initid->ptr;

	if (data->count == 0) {
		*is_null = 1;
		return 0;
	}
	return (data->c - data->x * data->y / data->count) / data->count;
}

inline static unsigned int doubleSlot(struct DoubleBuffer *buffer, double value, int m, int n)
{
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

inline static void doublePush(struct DoubleBuffer *buffer, unsigned int step, double value)
{
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

#define LESSSIZE()							\
	if (data->size == data->used) {			\
		data->size<<= 1;					\
		data->number = (double *) realloc(data->number, data->size * sizeof(*(data->number))); \
	}

#define LESSINIT()							\
											\
	if (NULL == (data = malloc(sizeof (*data)))) {	\
		strcpy(message, "Memory allocation failed"); \
		return 1;							\
	}										\
	data->used = 0;							\
	data->sum = 0;							\
	data->size = 32;						\
	data->number = (double *) malloc(data->size * sizeof(*(data->number))); \
											\
	initid->maybe_null = 1;					\
	initid->ptr = (char*) data

#define LESSCLEAR()							\
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
											\
	data->used = 0


#define LESSADD()							\
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
	double value = *((double*)args->args[0]); \
											\
	unsigned int s = doubleSlot(data, value, 0, data->used - 1); \
											\
	if (NULL == args->args[0])				\
		return;								\
											\
	LESSSIZE();								\
											\
	switch (type) {							\
	case 1:									\
		data->sum = data->sum + value;		\
		break;								\
	}										\
											\
	doublePush(data, s, value);				\
	data->used++


#define LESSDEINIT()						\
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr; \
											\
	if (NULL != data->number) {				\
		free(data->number);					\
	}										\
											\
	if (NULL != data) {						\
		free(data);							\
	}


#define LESS()								\
	do {									\
		psum+= data->number[count];			\
											\
		if (psum < limit) {					\
			count++;						\
											\
			if (count >= data->used)		\
				break;						\
											\
			continue;						\
		}									\
		break;								\
											\
	} while (1)

my_bool lesspart_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct DoubleBuffer* data = NULL;

	if (2 != args->arg_count) {
		strcpy(message, "lesspart must have exaclty two arguments");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;
	args->arg_type[1] = REAL_RESULT;

	LESSINIT();

	return 0;
}

void lesspart_clear(UDF_INIT* initid, char* is_null, char *error)
{
	LESSCLEAR();
}

void lesspart_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	int type = 0;
	LESSADD();
}

void lesspart_deinit(UDF_INIT *initid)
{
	LESSDEINIT();
}

longlong lesspart(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
	double limit = *((double*) args->args[1]);

	double psum = 0;

	ulonglong count = 0;

	if (limit < 0) {
		*error = 1;
		return 0;
	}

	LESS();

	return (longlong) count;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

my_bool lesspartpct_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct DoubleBuffer* data = NULL;

	if (2 != args->arg_count) {
		strcpy(message, "lesspartpct must have exaclty two arguments");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;
	args->arg_type[1] = REAL_RESULT;

	LESSINIT();

	return 0;
}

void lesspartpct_clear(UDF_INIT* initid, char* is_null, char *error)
{
	LESSCLEAR();
}

void lesspartpct_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	int type = 1;
	LESSADD();
}

void lesspartpct_deinit(UDF_INIT *initid)
{
	LESSDEINIT();
}

longlong lesspartpct(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
	double limit = *((double*) args->args[1]);

	double psum = 0;

	ulonglong count = 0;

	if (!(limit >= 0 && limit <= 1)) {
		*error = 1;
		return 0;
	}

	limit = limit * data->sum;

	LESS();

	return (longlong) count;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

my_bool lessavg_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct DoubleBuffer* data = NULL;

	if (1 != args->arg_count) {
		strcpy(message, "lessavg must have exactly one argument");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;

	LESSINIT();

	return 0;
}

void lessavg_clear(UDF_INIT* initid, char* is_null, char *error)
{
	LESSCLEAR();
}

void lessavg_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;
	double value = *((double*) args->args[0]);

	if (NULL == args->args[0])
		return;

	LESSSIZE();

	data->sum = data->sum + value;
	data->number[data->used] = value;
	data->used++;
}

void lessavg_deinit(UDF_INIT *initid)
{
	LESSDEINIT();
}

longlong lessavg(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

	ulonglong count = 0;

	double /* volatile */ *ptr;
	double limit = data->sum / data->used;
	double *beg = data->number;

	if (1 == data->used % 2) {
		count = (data->number[--data->used] < limit);
	}

	ptr = &(data->number[data->used]);

	while (beg < ptr) {
		count+= (*--ptr < limit);
		count+= (*--ptr < limit); // new line saves the volatile pointer
	}
	return (longlong) count;
}

#define XCHNG(a,b) { register double tmp = buf[a]; buf[a] = buf[b]; buf[b] = tmp; }

/*
 *  This algorithm is based on the description in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 */
static double __median(double *buf, int len)
{
	int low = 0,
			high = len - 1,
			median = high / 2,
			mid, ll, hh;

	do {

		if (high <= low)
			return buf[median];

		if (high == low + 1) {
			if (buf[high] < buf[low])
				XCHNG(low, high);
			return buf[median];
		}

		ll = low + 1;
		hh = high;

		mid = (low + high) / 2;
		if (buf[mid] > buf[high]) XCHNG(mid, high);
		if (buf[low] > buf[high]) XCHNG(low, high);
		if (buf[mid] > buf[low])  XCHNG(mid, low);

		XCHNG(mid, ll);

		do {
			do ++ll; while (buf[low] > buf[ll]);
			do --hh; while (buf[hh] > buf[low]);

			if (hh < ll)
				break;

			XCHNG(ll, hh);
		} while (1);

		XCHNG(low, hh);

		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;

	} while (1);
}
#undef XCHNG

my_bool median_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	struct DoubleBuffer* data = NULL;

	if (1 != args->arg_count) {
		strcpy(message, "lessavg must have exactly one argument");
		return 1;
	}

	args->arg_type[0] = REAL_RESULT;

	LESSINIT();

	return 0;
}

void median_clear(UDF_INIT* initid, char* is_null, char *error)
{
	LESSCLEAR();
}

void median_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

	if (NULL == args->args[0])
		return;

	double value = *((double*) args->args[0]);

	LESSSIZE();

	data->number[data->used] = value;
	data->used++;
}

void median_deinit(UDF_INIT *initid)
{
	LESSDEINIT();
}

double median(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	struct DoubleBuffer *data = (struct DoubleBuffer *) initid->ptr;

	if (data->used==0) {
		*is_null = 1;
		return 0;
	}
	return __median(data->number, data->used);
}

#endif /* HAVE_DLOPEN */
