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

#ifdef HAVE_DLOPEN

/* These must be right or mysqld will not find the symbol! */

my_bool isbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void isbit_deinit(UDF_INIT *initid);
long long isbit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool setbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void setbit_deinit(UDF_INIT *initid);
long long setbit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool invbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void invbit_deinit(UDF_INIT *initid);
long long invbit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool numbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void numbit_deinit(UDF_INIT *initid);
long long numbit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool msbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void msbit_deinit(UDF_INIT *initid);
long long msbit(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);


my_bool setint_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void setint_deinit(UDF_INIT *initid);
long long setint(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool getint_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void getint_deinit(UDF_INIT *initid);
long long getint(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);


my_bool bround_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void bround_deinit(UDF_INIT *initid);
long long bround(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool xround_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void xround_deinit(UDF_INIT *initid);
long long xround(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);


my_bool thumbscale_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void thumbscale_deinit(UDF_INIT *initid);
long long thumbscale(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool thumbratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void thumbratio_deinit(UDF_INIT *initid);
double thumbratio(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool starratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void starratio_deinit(UDF_INIT *initid);
double starratio(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);


my_bool bound_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void bound_deinit(UDF_INIT *initid);
double bound(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool sigfig_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void sigfig_deinit(UDF_INIT *initid);
double sigfig(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);


my_bool cut_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void cut_deinit(UDF_INIT *initid);
char *cut(UDF_INIT *initid, UDF_ARGS *args,
	char *result, unsigned long *length,
	char *is_null, char *error);

my_bool makeurl_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void makeurl_deinit(UDF_INIT *initid);
char *makeurl(UDF_INIT *initid, UDF_ARGS *args,
	char *result, unsigned long *length,
	char *is_null, char *error);

my_bool isbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count) {
	strcpy(message, "isbit must have exactly two arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long isbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{

    if (NULL == args->args[0] || NULL == args->args[1]) {
	*is_null = 1;
	return 0;
    }

    long long bit = *((longlong *) args->args[0]);
    long long n = *((longlong *) args->args[1]);

    return !!(bit & (1 << n));
}

my_bool setbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 2 || args->arg_count > 3) {
	strcpy(message, "setbit must have two or three arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long setbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1]) {
	*is_null = 1;
	return 0;
    }

    long long bit = *((longlong *) args->args[0]);
    long long n = *((longlong *) args->args[1]);

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

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long invbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1]) {
	*is_null = 1;
	return 0;
    }

    long long bit = *((longlong *) args->args[0]);
    long long n = *((longlong *) args->args[1]);

    return bit ^ (1 << n);
}

my_bool numbit_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (1 != args->arg_count && 2 != args->arg_count) {
	strcpy(message, "numbit must have one or two one arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 0;

    return 0;
}

long long numbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0]) {
	*is_null = 1;
	return 0;
    }

    short c;
    long long bit = *((longlong *) args->args[0]);

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

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long msbit(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0]) {
	*is_null = 1;
	return 0;
    }
    unsigned long long v = *((longlong *) args->args[0]);
    unsigned r = 0;

    /* O(n) but works with i64 without problems, maybe split into O(2log(n)) later*/
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

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long getint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2]) {
	*is_null = 1;
	return 0;
    }
    long long n = *((longlong *) args->args[0]);
    long long x = *((longlong *) args->args[1]);
    long long y = *((longlong *) args->args[2]);

    return (n >> x) & ((2 << (y - x)) - 1);
}

my_bool setint_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (4 != args->arg_count) {
	strcpy(message, "setint must have exactly four arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long setint(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2] || NULL == args->args[3]) {
	*is_null = 1;
	return 0;
    }
    long long n = *((longlong *) args->args[0]);
    long long x = *((longlong *) args->args[1]);
    long long y = *((longlong *) args->args[2]);
    long long m = *((longlong *) args->args[3]);

    return n & (~(((2 << (y - x)) - 1) << x)) | (m << x);
}

my_bool bround_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count) {
	strcpy(message, "bround must have exactly two arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

long long bround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    if (NULL == args->args[0] || NULL == args->args[1]) {
	*is_null = 1;
	return 0;
    }
    long long n = *((longlong *) args->args[0]);
    long long b = *((longlong *) args->args[1]);

    if (0 == b) {
	return 0;
    }

    return n + b - n % b;
}

my_bool xround_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (1 != args->arg_count) {
	strcpy(message, "xround must have exactly one single argument");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 0;

    return 0;
}

long long xround(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    static const long long map[] = {
	1000000000000000000LL,
	100000000000000000LL,
	10000000000000000LL,
	1000000000000000LL,
	100000000000000LL,
	10000000000000LL,
	1000000000000LL,
	100000000000LL,
	10000000000LL,
	1000000000LL,
	100000000LL,
	10000000LL,
	1000000LL,
	100000L,
	10000LL,
	1000LL,
	100LL,
	10LL,
	1LL
    };

    size_t i;

    if (NULL == args->args[0]) {
	*is_null = 1;
	return 0;
    }

    long long n = *((longlong *) args->args[0]);

    for (i = 1; i < 19; i++) {
	if (n > map[i]) {
	    return map[i - 1];
	}
    }
    return 1;
}

my_bool thumbscale_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (3 != args->arg_count) {
	strcpy(message, "thumbscale must have exactly three arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = 8;
    initid->max_length = 20;

    return 0;
}

long long thumbscale(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{

    if (NULL == args->args[0] || NULL == args->args[1] || NULL == args->args[2]) {
	*is_null = 1;
	return 0;
    }

    long long up = *((longlong *) args->args[0]);
    long long down = *((longlong *) args->args[1]);
    long long scale = *((longlong *) args->args[2]);

    return round(scale * up / (double) (down + up));
}

my_bool thumbratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count) {
	strcpy(message, "thumbratio must have exactly two arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = 8;
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

    long long up = *((longlong *) args->args[0]);
    long long down = *((longlong *) args->args[1]);

    return (double) (up + down) / (1 + abs(up - down));
}

my_bool starratio_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 2) {
	strcpy(message, "bound must have at least two arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = 8;
    initid->max_length = 20;

    return 0;
}

double starratio(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{
    size_t i = args->arg_count;
    longlong n = 0, d = 0;

    while(i--) {

	if (NULL == args->args[i]) {
	    *is_null = 1;
	    return 0;
	}
	n+= *((longlong *) args->args[i]) * ((longlong) i + 1);
	d+= *((longlong *) args->args[i]);
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
    initid->decimals = 8;
    initid->max_length = 20;

    return 0;
}

double bound(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{

    if (NULL == args->args[0]) {
	*is_null = 1;
	return 0;
    }

    if (NULL != args->args[1] && *((double *) args->args[0]) < *((double *) args->args[1])) {
	return *((double *) args->args[1]);
    }

    if (NULL != args->args[2] && *((double *) args->args[0]) > *((double *) args->args[2])) {
	return *((double *) args->args[2]);
    }
    return *((double *) args->args[0]);
}

my_bool sigfig_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count) {
	strcpy(message, "sigfig must have exactly two arguments");
	return 1;
    }

    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->decimals = 8;
    initid->max_length = 20;

    args->arg_type[0] = REAL_RESULT;

    return 0;
}

double sigfig(UDF_INIT *initid __attribute__((unused)), UDF_ARGS *args,
	char *is_null,
	char *error __attribute__((unused)))
{

    if (NULL == args->args[0] || NULL == args->args[1]) {
	*is_null = 1;
	return 0;
    }

    double value = *((double *) args->args[0]), pow_exp, pow_sig;
    long long figs = *((longlong *) args->args[1]);

    pow_exp = pow(10.0, (int) log10(value) + 1);
    pow_sig = pow(10.0, figs);

    return ((int) ((value / pow_exp) * pow_sig + 0.5) / pow_sig * pow_exp);
}

my_bool cut_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (2 != args->arg_count && 3 != args->arg_count) {
	strcpy(message, "cut must have two or three arguments");
	return 1;
    }

    args->arg_type[0] = STRING_RESULT;
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
    long sl = args->lengths[0], cl = 3, max = *((long long *) args->args[1]), i;

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
	*length += cl;
	return result;
    }

    memcpy(result, str, sl);
    *length = sl;
    result[*length] = 0;
    return result;
}

my_bool makeurl_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (1 != args->arg_count && 3 != args->arg_count) {
	strcpy(message, "makeurl must have exaclty one argument");
	return 1;
    }

    args->arg_type[0] = STRING_RESULT;

    initid->max_length = args->attribute_lengths[0] * 2;
    initid->const_item = 1;
    initid->maybe_null = 1;

    return 0;
}

char *makeurl(UDF_INIT *initid, UDF_ARGS *args,
	char *result, unsigned long *length,
	char *is_null, char *error __attribute__((unused)))
{
    char *start_res = result, *ptr = args->args[0], *end_str = ptr + args->lengths[0], add = 0;

    /* My own UTF8 and latin translation table */

    if (NULL == args->args[0]) {
	*is_null = 1;
	return 0;
    }

    for (; ptr < end_str; ptr++) {

	unsigned char c = *ptr;

	switch (c) {
	case 65 ... 90:
	    c |= 32;
	case 97 ... 122:
	case 48 ... 57:
	    *(result++) = c;
	    add = 1;
	    break;
	case 223:
	    *(result++) = 's';
	    *(result++) = 's';
	    add = 1;
	    break;
	case 196:   case 198:
	case 228:   case 230:
	    *(result++) = 'a';
	    *(result++) = 'e';
	    add = 1;
	    break;
	case 214:   case 246:
	    *(result++) = 'o';
	    *(result++) = 'e';
	    add = 1;
	    break;
	case 220:   case 252:
	    *(result++) = 'u';
	    *(result++) = 'e';
	    add = 1;
	    break;
	case 192:   case 193:
	case 194:   case 195:
	case 197:   case 224:
	case 225:   case 226:
	case 227:   case 229:
	    *(result++) = 'a';
	    add = 1;
	    break;
	case 200:   case 201:
	case 202:   case 203:
	case 232:   case 233:
	case 234:   case 235:
	    *(result++) = 'e';
	    add = 1;
	    break;
	case 161:   case 204:
	case 205:   case 206:
	case 207:   case 236:
	case 237:   case 238:
	case 239:
	    *(result++) = 'i';
	    add = 1;
	    break;
	case 210:   case 211:
	case 212:   case 213:
	case 240:   case 242:
	case 243:   case 244:
	case 245:
	    *(result++) = 'o';
	    add = 1;
	    break;
	case 181:   case 217:
	case 218:   case 219:
	case 249:   case 250:
	case 251:
	    *(result++) = 'u';
	    add = 1;
	    break;
	case 209:   case 241:
	    *(result++) = 'n';
	    add = 1;
	    break;
	case 162:   case 169:
	case 199:   case 231:
	    *(result++) = 'c';
	    add = 1;
	    break;
	case 222:   case 254:
	    *(result++) = 'p';
	    add = 1;
	    break;
	case 165:   case 221:
	case 253:   case 255:
	    *(result++) = 'y';
	    add = 1;
	    break;
	case 215:
	    *(result++) = 'x';
	    add = 1;
	    break;
	case 174:
	    *(result++) = 'r';
	    add = 1;
	    break;
	case 208:
	    *(result++) = 'd';
	    add = 1;
	    break;
	default:
	    switch (*((unsigned short *) ptr++)) {
	    case 0x9fc3:
		*(result++) = 's';
		*(result++) = 's';
		add = 1;
		break;
	    case 0x84c3: case 0xa4c3:
	    case 0xa6c3: case 0x86c3:
		*(result++) = 'a';
		*(result++) = 'e';
		add = 1;
		break;
	    case 0x96c3: case 0xb6c3:
		*(result++) = 'o';
		*(result++) = 'e';
		add = 1;
		break;
	    case 0x9cc3: case 0xbcc3:
		*(result++) = 'u';
		*(result++) = 'e';
		add = 1;
		break;
	    case 0xa0c3: case 0xa1c3:
	    case 0xa2c3: case 0xa3c3:
	    case 0xa5c3: case 0x80c3:
	    case 0x81c3: case 0x82c3:
	    case 0x83c3: case 0x85c3:
		*(result++) = 'a';
		add = 1;
		break;
	    case 0xa8c3: case 0xa9c3:
	    case 0xaac3: case 0xabc3:
	    case 0x88c3: case 0x89c3:
	    case 0x8ac3: case 0x8bc3:
		*(result++) = 'e';
		add = 1;
		break;
	    case 0xacc3: case 0xadc3:
	    case 0xaec3: case 0xafc3:
	    case 0x8cc3: case 0x8dc3:
	    case 0x8ec3: case 0x8fc3:
		*(result++) = 'i';
		add = 1;
		break;
	    case 0xb0c3: case 0xb2c3:
	    case 0xb3c3: case 0xb4c3:
	    case 0xb5c3: case 0x92c3:
	    case 0x93c3: case 0x94c3:
	    case 0x95c3:
		*(result++) = 'o';
		add = 1;
		break;
	    case 0xb9c3: case 0xbac3:
	    case 0xbbc3: case 0x99c3:
	    case 0x9ac3: case 0x9bc3:
	    case 0xb5c2:
		*(result++) = 'u';
		add = 1;
		break;
	    case 0x91c3: case 0xb1c3:
		*(result++) = 'n';
		add = 1;
		break;
	    case 0x87c3: case 0xa7c2:
	    case 0xa2c2: case 0xa7c3:
		*(result++) = 'c';
		add = 1;
		break;
	    case 0x9ec3: case 0xbec3:
		*(result++) = 'p';
		add = 1;
		break;
	    case 0xbfc3: case 0xa5c2:
	    case 0x9dc3: case 0xbdc3:
		*(result++) = 'y';
		add = 1;
		break;
	    case 0x97c3:
		*(result++) = 'x';
		add = 1;
		break;
	    case 0xaec2:
		*(result++) = 'r';
		add = 1;
		break;
	    case 0x90c3:
		*(result++) = 'd';
		add = 1;
		break;
	    default:
		if (add) {
		    *(result++) = '-';
		    add = 0;
		}
		ptr--;
	    }
	}
    }

    if ((*length = result - start_res) > 0 && *(result - 1) == '-') {
	result--;
    }

    *result = 0;

    return start_res;
}

#endif /* HAVE_DLOPEN */
