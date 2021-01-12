#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// CRC-16/X25 模型参数
#define CRC_CODE_SIZE 16
#define CRC_INIT_VAL  0xFFFF
#define CRC_POLY_VAL  0x1021
#define CRC_OXOR_VAL  0xFFFF
#define CRC_IN_REV    1
#define CRC_OUT_REV   1

static uint8_t reverse_8bits(uint8_t x)
{
    x = ((x & 0xaa) >> 1) | ((x & 0x55) << 1);
    x = ((x & 0xcc) >> 2) | ((x & 0x33) << 2);
    x = ((x & 0xf0) >> 4) | ((x & 0x0f) << 4);
    return x;
}

static uint16_t reverse_16bits(uint16_t x)
{
    x = ((x & 0xaaaa) >> 1) | ((x & 0x5555) << 1);
    x = ((x & 0xcccc) >> 2) | ((x & 0x3333) << 2);
    x = ((x & 0xf0f0) >> 4) | ((x & 0x0f0f) << 4);
    x = ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8);
    return x;
}

static uint32_t reverse_32bits(uint32_t x)
{
    x = ((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1);
    x = ((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2);
    x = ((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4);
    x = ((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8);
    return ((x >> 16) | (x << 16));
}

uint32_t crc(uint8_t *buf, int len)
{
    uint32_t c = CRC_INIT_VAL;
    uint8_t  b;
    int      i;
    while (len-- > 0) {
        b  = *buf++;
        if (CRC_IN_REV) b = reverse_8bits(b);
        c ^=  b << (CRC_CODE_SIZE - 8);
        for (i=0; i<8; i++) {
            c = (c & (1 << (CRC_CODE_SIZE - 1))) ? ((c << 1) ^ CRC_POLY_VAL) : (c << 1);
        }
    }
    if (CRC_OUT_REV) {
        switch (CRC_CODE_SIZE) {
        case 8 : c = reverse_8bits (c); break;
        case 16: c = reverse_16bits(c); break;
        case 32: c = reverse_32bits(c); break;
        }
    }
    return (c ^ CRC_OXOR_VAL) & ((1uL << CRC_CODE_SIZE) - 1);
}

int main(void)
{
    uint8_t data1[] = { 0x3F, 0xF7, 0x4E, 0x7C, 0x2A, 0x79, 0x10, 0x28, 0x59, 0x9F, 0x75, 0xBE, 0x8D, 0xE9, 0x93, 0x38 };
    uint8_t data2[] = { 0xF5, 0x51, 0x0D, 0x3F, 0x2F, 0x48, 0xA4, 0x44, 0x0E, 0x25, 0x61, 0x65, 0x72, 0xD8, 0x32, 0xD4 };
    uint8_t data3[] = { 0xA2, 0x9E, 0xF3, 0x34, 0xD0, 0xC8, 0xC5, 0x81, 0x35, 0x08, 0x4F, 0x15, 0x65, 0x9F, 0x34, 0x6C };
    uint8_t data4[] = { 0x29, 0xDA, 0x73, 0x7E, 0x46, 0x27, 0xB6, 0x78, 0x52, 0x8F, 0xF8, 0xCD, 0x5F, 0x42, 0xE2, 0x7E };
    uint8_t data5[] = { 0xB1, 0x68, 0x88, 0x1F, 0xFD, 0x08, 0x84, 0x3E, 0xA2, 0xC2, 0x1D, 0x93, 0x65, 0xF0, 0x4B, 0xCB };
    uint8_t data6[] = { 0x88, 0x92, 0x11, 0x4E, 0x23, 0xB8, 0xC6, 0x77, 0xDA, 0x88, 0x12, 0x72, 0x07, 0xCC, 0x67, 0x0F };

    printf("%x\n", crc(data1, sizeof(data1)));
    printf("%x\n", crc(data2, sizeof(data2)));
    printf("%x\n", crc(data3, sizeof(data3)));
    printf("%x\n", crc(data4, sizeof(data4)));
    printf("%x\n", crc(data5, sizeof(data5)));
    printf("%x\n", crc(data6, sizeof(data6)));
    return 0;
}


