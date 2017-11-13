#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;

static uint8_t base64_to_char(uint8_t v)
{
    if (v < 26) {
        return 'A' + (v - 0 );
    } else if (v < 52) {
        return 'a' + (v - 26);
    } else if (v < 62) {
        return '0' + (v - 52);
    } else if (v == 62) {
        return '+';
    } else {
        return '/';
    }
}

static uint8_t char_to_base64(uint8_t c, int *padding)
{
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 0;
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    } else if (c == '=') {
        (*padding)++;
        return 0;
    } else {
        return -1;
    }
}

static void base64_encode_3bytes(uint8_t *dst, uint8_t *src)
{
    uint32_t data = (src[0] << 16) | (src[1] << 8) | (src[2] << 0);
    *dst++ = base64_to_char((data >> 18) & 0x3f);
    *dst++ = base64_to_char((data >> 12) & 0x3f);
    *dst++ = base64_to_char((data >>  6) & 0x3f);
    *dst++ = base64_to_char((data >>  0) & 0x3f);
}

static int base64_decode_4bytes(uint8_t *dst, uint8_t *src)
{
    uint32_t data    = 0;
    int      padding = 0;
    data |= char_to_base64(*src++, &padding); data <<= 6;
    data |= char_to_base64(*src++, &padding); data <<= 6;
    data |= char_to_base64(*src++, &padding); data <<= 6;
    data |= char_to_base64(*src++, &padding); data <<= 0;
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
    uint8_t buf[4];

    if (argc < 4) {
        printf(
            "+-----------+\n"
            " base64 tool \n"
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
            ret = fread(buf, 1, 3, fpsrc);
            if (ret <= 0 || ret > 3) break;
            padding = 3 - ret;
            memset(buf + ret, 0, padding);
            base64_encode_3bytes(buf, buf);
            fwrite(buf , 1, 4 - padding, fpdst);
            fwrite("==", 1, padding    , fpdst);
            if (++counter % 16 == 0) {
                fwrite("\r\n", 1, 2, fpdst);
            }
        }
    } else {
        while (1) {
            ret = read_text_bytes(buf, 4, fpsrc);
            if (ret <= 0 || ret > 4) break;
            padding = base64_decode_4bytes(buf, buf);
            fwrite(buf, 1, 3 - padding, fpdst);
        }
    }

done:
    if (fpsrc) fclose(fpsrc);
    if (fpdst) fclose(fpdst);
    return ret;
}






