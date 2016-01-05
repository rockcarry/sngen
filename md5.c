// 包含头文件
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 类型定义
typedef unsigned char      uint8_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

// 常量定义
#define F(x, y, z)  ((x & y) | (~x & z))
#define G(x, y, z)  ((x & z) | ( y &~z))
#define H(x, y, z)  (x^y^z)
#define I(x, y, z)  (y^(x|~z))

// 内部函数
static uint32_t rotate_left(uint32_t x, int s) { return (x << s) | (x >> (32 - s)); }
static uint32_t FF(uint32_t *w, uint32_t x, int s, uint32_t t) { return (w[1] + rotate_left(w[0] + F(w[1], w[2], w[3]) + x + t, s)); }
static uint32_t GG(uint32_t *w, uint32_t x, int s, uint32_t t) { return (w[1] + rotate_left(w[0] + G(w[1], w[2], w[3]) + x + t, s)); }
static uint32_t HH(uint32_t *w, uint32_t x, int s, uint32_t t) { return (w[1] + rotate_left(w[0] + H(w[1], w[2], w[3]) + x + t, s)); }
static uint32_t II(uint32_t *w, uint32_t x, int s, uint32_t t) { return (w[1] + rotate_left(w[0] + I(w[1], w[2], w[3]) + x + t, s)); }

// 函数实现
void md5_digest(uint8_t md5[16], uint8_t *data, int len)
{
    int hlen = len & ~0x3f;
    int llen = len &  0x3f;

    uint32_t W[4], WW[9], X[16], *MW, *t;
    uint32_t padding_buf[32];
    int      padding_flag = llen < 56 ? 1 : 2;
    int      process_flag = 0;
    int      i, k;
    uint8_t *psrc, *pend;

    static uint32_t T[64] =
    {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };
    static int SFF[4] = { 7, 12, 17, 22 };
    static int SGG[4] = { 5, 9 , 14, 20 };
    static int SHH[4] = { 4, 11, 16, 23 };
    static int SII[4] = { 6, 10, 15, 21 };

    // init W & MW
    W[0] = 0x67452301; W[1] = 0xefcdab89; W[2] = 0x98badcfe; W[3] = 0x10325476;
    MW   = (uint32_t*)WW + 4;

    // append padding bits & length to padding buffer
    memcpy((uint8_t*)padding_buf + 0   , data + hlen,       llen);
    memset((uint8_t*)padding_buf + llen, 0          , 128 - llen);
    ((uint8_t *)padding_buf)[llen] = 0x80;
    ((uint64_t*)padding_buf)[8 * padding_flag - 1] = len * 8;

process_handler:
    // init psrc, pend
    if (!process_flag) { psrc = data; pend = psrc + hlen; }
    else { psrc = (uint8_t*)padding_buf; pend = psrc + 64 * padding_flag; }

    while (psrc < pend)
    {
        // copy 512 bits data into X.
        memcpy(X, psrc, 64); psrc += 64;

        // save W
        for (i=0; i<4; i++) WW[i+0] = WW[i+4] = W[i];

        t = T; // init t
        for (i=0,k=0; i<16; i++,k+=1,k&=0xf) MW[-(i&3)] = MW[4-(i&3)] = FF(&(MW[-(i&3)]), X[k], SFF[i&3], *t++); // round 1
        for (i=0,k=1; i<16; i++,k+=5,k&=0xf) MW[-(i&3)] = MW[4-(i&3)] = GG(&(MW[-(i&3)]), X[k], SGG[i&3], *t++); // round 2
        for (i=0,k=5; i<16; i++,k+=3,k&=0xf) MW[-(i&3)] = MW[4-(i&3)] = HH(&(MW[-(i&3)]), X[k], SHH[i&3], *t++); // round 3
        for (i=0,k=0; i<16; i++,k+=7,k&=0xf) MW[-(i&3)] = MW[4-(i&3)] = II(&(MW[-(i&3)]), X[k], SII[i&3], *t++); // round 4

        // increase W
        for (i=0; i<4; i++) W[i] += MW[i];
    }

    // goto process_handler if necessary
    if (!process_flag) { process_flag = 1; goto process_handler; }

    // copy result
    memcpy(md5, W, 16);
}

int main(void)
{
    uint8_t md5[16];
    char   *str = "hello";
    int     i;

    md5_digest(md5, (uint8_t*)str, (int)strlen(str));
    for (i=0; i<16; i++) printf("%02X", md5[i]); printf("\n");
    return 0;
}


