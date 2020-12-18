// 包含头文件
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "md5.h"

// 常量定义
#define MD5_BLOCK_SIZE  64
#define F(x, y, z)  ((x & y) | (~x & z))
#define G(x, y, z)  ((x & z) | ( y &~z))
#define H(x, y, z)  (x ^ y ^ z)
#define I(x, y, z)  (y ^ (x | ~z))

// 内部函数
static uint32_t rotate_left(uint32_t x, int s) { return (x << s) | (x >> (32 - s)); }
static uint32_t FF(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t x, int s, uint32_t t) { return (w1 + rotate_left(w0 + F(w1, w2, w3) + x + t, s)); }
static uint32_t GG(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t x, int s, uint32_t t) { return (w1 + rotate_left(w0 + G(w1, w2, w3) + x + t, s)); }
static uint32_t HH(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t x, int s, uint32_t t) { return (w1 + rotate_left(w0 + H(w1, w2, w3) + x + t, s)); }
static uint32_t II(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t x, int s, uint32_t t) { return (w1 + rotate_left(w0 + I(w1, w2, w3) + x + t, s)); }

static void md5_process_block(uint32_t w[4], uint32_t x[16])
{
    static const uint32_t T[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };
    static const int SFF[4] = { 7, 12, 17, 22 };
    static const int SGG[4] = { 5, 9 , 14, 20 };
    static const int SHH[4] = { 4, 11, 16, 23 };
    static const int SII[4] = { 6, 10, 15, 21 };
    uint32_t wt[4] = { w[0], w[1], w[2], w[3] }, *t, i, k;
    t = (uint32_t*)T;
    for (i=0,k=0; i<16; i++,k+=1) wt[(4-i)&3] = FF(wt[(4-i)&3], wt[(1-i)&3], wt[(2-i)&3], wt[(3-i)&3], x[k&0xf], SFF[i&3], *t++); // round 1
    for (i=0,k=1; i<16; i++,k+=5) wt[(4-i)&3] = GG(wt[(4-i)&3], wt[(1-i)&3], wt[(2-i)&3], wt[(3-i)&3], x[k&0xf], SGG[i&3], *t++); // round 2
    for (i=0,k=5; i<16; i++,k+=3) wt[(4-i)&3] = HH(wt[(4-i)&3], wt[(1-i)&3], wt[(2-i)&3], wt[(3-i)&3], x[k&0xf], SHH[i&3], *t++); // round 3
    for (i=0,k=0; i<16; i++,k+=7) wt[(4-i)&3] = II(wt[(4-i)&3], wt[(1-i)&3], wt[(2-i)&3], wt[(3-i)&3], x[k&0xf], SII[i&3], *t++); // round 4
    for (i=0; i<4; i++) w[i] += wt[i];
}

typedef struct {
    uint32_t w[4];
    uint8_t  buffer[MD5_BLOCK_SIZE];
    uint32_t size_total;
    uint32_t size_buffer;
} CONTEXT;

void* md5_init(void)
{
    CONTEXT *context = calloc(1, sizeof(CONTEXT));
    if (!context) return NULL;
    md5_reset(context);
    return context;
}

void md5_free (void *ctxt) { if (ctxt) free(ctxt); }

void md5_reset(void *ctxt)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!ctxt) return;
    context->w[0] = 0x67452301;
    context->w[1] = 0xefcdab89;
    context->w[2] = 0x98badcfe;
    context->w[3] = 0x10325476;
    context->size_total = context->size_buffer = 0;
}

void md5_data(void *ctxt, uint8_t *data, int size)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!ctxt) return;
    while (size > 0) {
        int n = size < MD5_BLOCK_SIZE - context->size_buffer ? size : MD5_BLOCK_SIZE - context->size_buffer;
        memcpy(context->buffer + context->size_buffer, data, n);
        data += n; size -= n; context->size_buffer += n; context->size_total += n;
        if (context->size_buffer == MD5_BLOCK_SIZE) {
            md5_process_block(context->w, (uint32_t*)context->buffer);
            context->size_buffer = 0;
        }
    }
}

void md5_done(void *ctxt, uint8_t md5[16])
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!ctxt) return;
    memset(context->buffer + context->size_buffer, 0, MD5_BLOCK_SIZE - context->size_buffer);
    context->buffer[context->size_buffer] = 0x80;
    if (context->size_buffer > 56) {
        md5_process_block(context->w, (uint32_t*)context->buffer);
        memset(context->buffer, 0, MD5_BLOCK_SIZE);
    }
    ((uint64_t*)context->buffer)[7] = context->size_total * 8;
    md5_process_block(context->w, (uint32_t*)context->buffer);
    memcpy(md5, context->w, sizeof(context->w));
    md5_reset(ctxt);
}

#ifdef _TEST1_
int main(void)
{
    void   *md5ctx = md5_init();
    uint8_t md5sum[16];
    int     i;
    md5_data(md5ctx, (uint8_t*)"hello", 5);
    md5_data(md5ctx, (uint8_t*)" "    , 1);
    md5_data(md5ctx, (uint8_t*)"world", 5);
    md5_done(md5ctx, md5sum);
    for (i=0; i<16; i++) printf("%02X", md5sum[i]);
    printf("\n");
    md5_free(md5ctx);
    return 0;
}
#endif

