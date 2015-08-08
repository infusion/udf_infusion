#include "common.h"


#if 1
#  define EN(n)     (n)
#else
#  define EN(n)     (((n) >> 8) | (((n) << 8) & 0xffff))
#endif

#define ADD1(s)     *(result++) = s[0]; add = 1;
#define ADD1I(s)    ADD1(s); ++ptr;

#define ADD2(s)     *(result++) = s[0]; *(result++) = s[1]; add = 1;
#define ADD2I(s)    ADD2(s); ++ptr;

#define BTW(c, a, b)    (unsigned)(c) - (unsigned)(a) <= (unsigned)(b) - (unsigned)(a)


char *_translate_string(UDF_ARGS *args, char *result, unsigned long *length, char separator)
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
            case EN(0x95c3): case EN(0x98c3):
            case EN(0xb8c3):
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
