#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;

static const uint8_t BASE85_ENTAB[85] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z',
    '!', '#', '$', '%', '&', '(', ')', '*', '+', '-',
    ';', '<', '=', '>', '?', '@', '^', '_', '`', '{',
    '|', '}', '~',
};

static const uint8_t BASE85_DETAB[94] = {
    62,  0, 63, 64, 65, 66,  0, 67, 68, 69, 70,  0, 71,  0,  0,  0,
     1,  2,  3,  4,  5,  6,  7,  8,  9,  0, 72, 73, 74, 75, 76, 77,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35,  0,  0,  0, 78, 79, 80,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84,
};

static void base85_encode_4bytes(uint8_t *dst, uint8_t *src)
{
    uint32_t data = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | (src[3] << 0);
     dst  += 4;
    *dst-- = BASE85_ENTAB[data % 85]; data /= 85;
    *dst-- = BASE85_ENTAB[data % 85]; data /= 85;
    *dst-- = BASE85_ENTAB[data % 85]; data /= 85;
    *dst-- = BASE85_ENTAB[data % 85]; data /= 85;
    *dst-- = BASE85_ENTAB[data % 85]; data /= 1;
}

static int base85_decode_5bytes(uint8_t *dst, uint8_t *src)
{
    uint32_t data    = 0;
    int      padding = 0;
    data += BASE85_DETAB[*src - '!']; data *= 85; padding += (*src++ == '.');
    data += BASE85_DETAB[*src - '!']; data *= 85; padding += (*src++ == '.');
    data += BASE85_DETAB[*src - '!']; data *= 85; padding += (*src++ == '.');
    data += BASE85_DETAB[*src - '!']; data *= 85; padding += (*src++ == '.');
    data += BASE85_DETAB[*src - '!']; data *= 1;  padding += (*src++ == '.');
    *dst++ = (data >> 24) & 0xff;
    *dst++ = (data >> 16) & 0xff;
    *dst++ = (data >>  8) & 0xff;
    *dst++ = (data >>  0) & 0xff;
    return padding;
}

static int read_text_bytes(uint8_t *buf, int n, FILE *fp)
{
    int i = 0;
    while (i < n) {
        int c = fgetc(fp);
        if (c == EOF) break;
        if (c < '!' || c > '~') continue;
        buf[i++] = c;
    }
    return i;
}

int main(int argc, char *argv[])
{
    FILE *fpsrc;
    FILE *fpdst;
    int   encode  =  0;
    int   padding =  0;
    int   counter =  0;
    int   ret     = -1;
    uint8_t buf[5];

    if (argc < 4) {
        printf(
            "+-----------+\n"
            " base85 tool \n"
            "+-----------+\n"
            "usage:\n"
            "  decoding: %s -d input output\n"
            "  encoding: %s -e input output\n",
            argv[0], argv[0]
        );
        return 0;
    }

    fpsrc = fopen(argv[2], "rb");
    fpdst = fopen(argv[3], "wb");
    if (!fpsrc || !fpdst) {
        printf("failed to open file(s) !\n");
        goto done;
    }

    encode = !strcmp("-e", argv[1]);
    if (encode) {
        while (1) {
            ret = fread(buf, 1, 4, fpsrc);
            if (ret <= 0 || ret > 4) break;
            padding = 4 - ret;
            memset(buf + ret, 0, padding);
            base85_encode_4bytes(buf, buf);
            fwrite(buf  , 1, 5 - padding, fpdst);
            fwrite("...", 1, padding    , fpdst);
            if (++counter % 16 == 0) {
                fwrite("\r\n", 1, 2, fpdst);
            }
        }
    } else {
        while (1) {
            ret = read_text_bytes(buf, 5, fpsrc);
            if (ret <= 0 || ret > 5) break;
            padding = base85_decode_5bytes(buf, buf);
            fwrite(buf, 1, 4 - padding, fpdst);
        }
    }

done:
    if (fpsrc) fclose(fpsrc);
    if (fpdst) fclose(fpdst);
    return ret;
}






