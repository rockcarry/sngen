#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t my_mul(uint32_t a, uint32_t b)
{
    uint32_t c = 0, i;
    for (i=0; i<32; i++) {
        if (b & (1 << i)) c += (a << i);
    }
    return c;
}

uint32_t my_div(uint32_t a, uint32_t b, uint32_t *r)
{
    uint32_t c = 0;
    int      i;

    if      (b == 0) { printf("div by zero !\n"); return -1; }
    else if (a <  b) { if (r) *r = a; return 0; }
    else if (a == b) { if (r) *r = 0; return 1; }

    for (i = 0; i < 32 && (a > (b << i)); i++);
    if (a == (b << i)) { if (r) *r = 0; return (1 << i); }

    do {i--;
        if (a >= (b << i)) {
            a -= (b << i);
            c += (1 << i);
        }
    } while (i);

    if (r) *r = a;
    return c;
}

int main(void)
{
    uint32_t a, b, i;
    printf("%d, %d\n", 312456 * 789, my_mul(312456, 789));
    printf("%d, %d\n", 312456 / 789, my_div(312456, 789, NULL));
    fflush(stdout);

    for (i=0; i<0xFFFFFFF; i++) {
        a = rand();
        b = rand();
        if (a * b != my_mul(a, b)) {
            printf("a: %d, b: %d, %d, %d\b", a, b, a * b, my_mul(a, b));
        }
        if (b != 0 && a / b != my_div(a, b, NULL)) {
            printf("a: %d, b: %d, %d, %d\b", a, b, a / b, my_div(a, b, NULL));
        }
    }
    return 0;
}

