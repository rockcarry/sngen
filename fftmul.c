#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fft.h"

#define FFT_LEN  256

static void complex_mul(float *r, float *c1, float *c2)
{
    r[0] = c1[0] * c2[0] - c1[1] * c2[1];
    r[1] = c1[1] * c2[0] + c1[0] * c2[1];
}

static void digitalstr2complex(float *complex, int len, char *str)
{
    int n = strlen(str), i;
    if (n < len) memset(complex + n * 2, 0, (len - n) * sizeof(float) * 2);
    for (i=n-1; i>=0; i--) {
        if (str[i] >= '0' && str[i] <= '9') complex[0] = str[i] - '0';
        else complex[0] = 0;
        complex[1] = 0;
        complex   += 2;
    }
}

int main(int argc, char *argv[])
{
    char *stra = "12341234234";
    char *strb = "234594350";
    void *fftf, *ffti;
    float fa[FFT_LEN * 2], fb[FFT_LEN * 2], fc[FFT_LEN * 2];
    int   out[FFT_LEN], carry, flag = 0, i;

    if (argc > 2) { stra = argv[1]; strb = argv[2]; }
    digitalstr2complex(fa, FFT_LEN, stra);
    digitalstr2complex(fb, FFT_LEN, strb);

    fftf = fft_init(FFT_LEN, 0);
    ffti = fft_init(FFT_LEN, 1);
    if (fftf && ffti) {
        fft_execute(fftf, fa, fa);
        fft_execute(fftf, fb, fb);
        for (i=0; i<FFT_LEN; i++) complex_mul(fc + i * 2, fa + i * 2, fb + i * 2);
        fft_execute(ffti, fc, fc);
    }
    fft_free(fftf);
    fft_free(ffti);

    for (carry=0,i=0; i<FFT_LEN; i++) {
        out[i] = (int)(fc[i * 2] + 0.5) + carry;
        carry  = out[i] / 10;
        out[i]%= 10;
    }
    for (i=FFT_LEN-1; i>=0; i--) {
        if (flag == 0) flag = !!out[i];
        if (flag) printf("%d", out[i]);
    }
    printf("\n");
    return 0;
}
