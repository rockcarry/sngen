#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LNUM_SIZE  1024
#define DUMP_LNUM      0

#if DUMP_LNUM
static void dump_lnum_buf(const char *comment, char *lnum)
{
    int i;
    printf("%s: ", comment);
    for (i=0; i<MAX_LNUM_SIZE; i++) {
        printf("%2d ", lnum[i]);
    }
    printf("\n");
}
#endif

// handle carry of large number
static void lnum_carry(char *a, int start, int end)
{
    int i, c = 0;

#if DUMP_LNUM
    dump_lnum_buf("+lnum_carry", a);
#endif

    start = start > 0 ? start : 0;
    end   = end   > 0 ? end   : 0;
    for (i=start; i>=end; i--) {
        a[i] += c; 
        c     = a[i] / 10;
        a[i] %= 10;
    }
    if (c && i >= 0) a[i] = c;

#if DUMP_LNUM
    dump_lnum_buf("-lnum_carry", a);
#endif
}

static void lnum_to_string(char *str, char *lnum)
{
    int i;
    for (i=0; i<MAX_LNUM_SIZE && !lnum[i]; i++);
    if (i==MAX_LNUM_SIZE) *str++ = '0';
    for (   ; i<MAX_LNUM_SIZE; i++) *str++ = lnum[i] + '0';
    *str = '\0';
}

void lnum_multiply(char *result, char *a, char *b)
{
    char *lr = NULL;
    int   nda= strlen(a);
    int   ndb= strlen(b);
    int   i, j;

    lr = calloc(1, MAX_LNUM_SIZE);
    if (!lr) {
        printf("failed to allocate memory !\n");
        goto exit;
    }

    if (nda < ndb) { // swap a & b
        char *p;
        int   t;
        p = a; t   = nda;
        a = b; nda = ndb;
        b = p; ndb = t;
    }

    for (i=0; i<ndb; i++) {
        for (j=0; j<nda; j++) {
            if (MAX_LNUM_SIZE - i - j - 1 >= 0) {
                lr[MAX_LNUM_SIZE - i - j - 1] += (a[nda-j-1] - '0') * (b[ndb-i-1] - '0');
            }
        }
        lnum_carry(lr, MAX_LNUM_SIZE - i - 1, MAX_LNUM_SIZE - i - j);
    }

    lnum_to_string(result, lr);

exit:
    if (lr) free(lr);
}

#if 1
int main(int argc, char *argv[])
{
    char result[MAX_LNUM_SIZE+1] = {0};
    if (argc < 3) {
        printf("large number multiply program v1.0\n");
        printf("written by rockcarry@163.com\n");
        printf("usage: multiply 123 456\n");
        return 0;
    }
    lnum_multiply(result, argv[1], argv[2]);
    printf("%s * %s = %s\n", argv[1], argv[2], result);
}
#endif


