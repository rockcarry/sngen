#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* alaw
 * -------------  --------  ---------------
 * 0000000wxyza   000wxyz   0000000wxyz1
 * 0000001wxyza   001wxyz   0000001wxyz1
 * 000001wxyzab   010wxyz   000001wxyz10
 * 00001wxyzabc   011wxyz   00001wxyz100
 * 0001wxyzabcd   100wxyz   0001wxyz1000
 * 001wxyzabcde   101wxyz   001wxyz10000
 * 01wxyzabcdef   110wxyz   01wxyz100000
 * 1wxyzabcdefg   111wxyz   1wxyz1000000
 */
uint8_t pcm2alaw(int16_t pcm)
{
    uint8_t sign = (pcm >> 8) & (1 << 7);
    int     mask, eee, wxyz, alaw;
    if (sign) pcm = -pcm;
    for (mask=(1<<14),eee=7; (pcm&mask)==0&&eee>0; eee--,mask>>=1);
    wxyz  = (pcm >> ((eee == 0) ? 4 : (eee + 3))) & 0xf;
    alaw  = sign | (eee << 4) | wxyz;
    return (alaw ^ 0xd5);
}

int16_t alaw2pcm(uint8_t alaw)
{
    int sign, exponent, data;
    alaw    ^= 0xd5;
    sign     = alaw & 0x80;
    exponent = (alaw & 0x70) >> 4;
    data     = alaw & 0x0f;
    data   <<= 4;
    data    += 8;
    if (exponent != 0) data  += 0x100;
    if (exponent  > 1) data <<= (exponent - 1);
    return (int16_t)(sign == 0 ? data : -data);
}

/* ulaw
 * --------------  -------- ---------------
 * 00000001wxyza   000wxyz  00000001wxyz1
 * 0000001wxyzab   001wxyz  0000001wxyz10
 * 000001wxyzabc   010wxyz  000001wxyz100
 * 00001wxyzabcd   011wxyz  00001wxyz1000
 * 0001wxyzabcde   100wxyz  0001wxyz10000
 * 001wxyzabcdef   101wxyz  001wxyz100000
 * 01wxyzabcdefg   110wxyz  01wxyz1000000
 * 1wxyzabcdefgh   111wxyz  1wxyz10000000
 */
uint8_t pcm2ulaw(int16_t pcm)
{
    uint8_t sign = (pcm >> 8) & (1 << 7);
    int     mask, eee, wxyz, ulaw;
    if (sign) pcm = -pcm;
#if 1 // enable biased linear input
    pcm  = pcm < 0x7f7b ? pcm : 0x7f7b;
    pcm += 0x84;
#endif
    for (mask=(1<<14),eee=7; (pcm&mask)==0&&eee>0; eee--,mask>>=1);
    wxyz  = (pcm >> (eee + 3)) & 0xf;
    ulaw  = sign | (eee << 4) | wxyz;
    return (ulaw ^ 0xff);
}

int16_t ulaw2pcm(uint8_t ulaw)
{
    int t;
    ulaw ^= 0xff;
    t     = (ulaw & 0x0f) << 3;
    t    |= 0x84;
    t   <<= (ulaw & 0x70) >> 4;
    return (ulaw & (1 << 7)) ? (0x84 - t) : (t - 0x84);
}

#ifdef _TEST_
int main(void)
{
    int i;
    for (i=-0x7fff; i<=0x7fff; i++) {
        printf("pcm: %6d, alaw: %02x, ulaw: %02x\n", i, pcm2alaw(i), pcm2ulaw(i));
    }
    printf("\n");
    for (i=0; i<=0xff; i++) {
        printf("alaw: %02x, pcm: %6d   ulaw: %02x, pcm: %6d\n", i, alaw2pcm(i), i, ulaw2pcm(i));
    }
    printf("\n");
}
#endif

