// 包含头文件
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

// 置换选择一
static const int PC_1[56] =
{
    56, 48, 40, 32, 24, 16, 8 ,
    0 , 57, 49, 41, 33, 25, 17,
    9 , 1 , 58, 50, 42, 34, 26,
    18, 10, 2 , 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,
    6 , 61, 53, 45, 37, 29, 21,
    13, 5 , 60, 52, 44, 36, 28,
    20, 12, 4 , 27, 19, 11, 3 ,
};

// 置换选择二
static const int PC_2[48] =
{
    13, 16, 10, 23, 0 , 4 , 2 , 27,
    14, 5 , 20, 9 , 22, 18, 11, 3 ,
    25, 7 , 15, 6 , 26, 19, 12, 1 ,
    40, 51, 30, 36, 46, 54, 29, 39,
    50, 44, 32, 46, 43, 48, 38, 55,
    33, 52, 45, 41, 49, 35, 28, 31,
};

// 循环左移位数表
static const int SHIFT_TIMES[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

// 初始置换表 IP
static int IP_Table[64] =
{
    57, 49, 41, 33, 25, 17, 9 , 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7,
    56, 48, 40, 32, 24, 16, 8 , 0,
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
};

// 逆向置换表 FP
static const int FP_Table[64] =
{
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9 , 49, 17, 57, 25,
    32, 0, 40, 8 , 48, 16, 56, 24,
};

// 置换表 E
static const int E_Table[48] =
{
    31, 0 , 1 , 2 , 3 , 4 ,
    3 , 4 , 5 , 6 , 7 , 8 ,
    7 , 8 , 9 , 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31, 0 ,
};

// 置换表 P
static const int P_Table[32] =
{
    15, 6 , 19, 20, 28, 11, 27, 16,
    0 , 14, 22, 25, 4 , 17, 30, 9 ,
    1 , 7 , 23, 13, 31, 26, 2 , 8 ,
    18, 12, 29, 5 , 21, 10, 3 , 24,
};

// S-BOX
static const int S[8][4][16] =
{
    { // S1
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13},
    },

    { // S2
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9},
    },

    { // S3
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12},
    },

    { // S4
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14},
    },

    { // S5
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3},
    },

    { // S6
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13},
    },

    { // S7
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12},
    },

    { // S8
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11},
    },
};

static uint64_t des_transform(uint64_t data, const int *tab, int n)
{
    uint64_t temp = 0;
    int      i;

    for (i=0; i<=n; i++) {
        if (data & (1LL << tab[n - i])) temp |= (1LL << (n - i));
    }
    return temp;
}

#define DES_PC1_TRANSFORM(data) des_transform(data, PC_1, 55)
#define DES_PC2_TRANSFORM(data) des_transform(data, PC_2, 47)
#define DES_IP_TRANSFORM(data)  des_transform(data, IP_Table, 63)
#define DES_FP_TRANSFORM(data)  des_transform(data, FP_Table, 63)
#define DES_E_TRANSFORM(data)   des_transform(data, E_Table , 47)
#define DES_P_TRANSFORM(data)   des_transform(data, P_Table , 31)

static uint64_t des_rol(uint64_t data, int n)
{
    uint64_t bitshigh;
    uint64_t bitsmask;

    bitsmask   = (1LL << n) - 1;
    bitshigh   = (data & bitsmask);
    bitshigh <<= (56 - n);
    return (data >> n) | bitshigh;
}

static uint64_t des_sbox(uint64_t data)
{
    uint64_t temp = 0;
    int      row;
    int      col;
    int      i;

    for (i=0; i<8; i++)
    {
        row    = ((data & (1LL << 47)) >> 46) | ((data & (1LL << 42)) >> 42);
        col    = ((data & (0xfLL << 43)) >> 43);
        data <<= 6;
        temp <<= 4;
        temp  |= S[i][row][col];
    }
    return temp;
}

void des_make_subkeys(uint64_t key, uint64_t subkeys[16])
{
    uint64_t temp;
    int      i;

    // PC1 置换
    temp = DES_PC1_TRANSFORM(key);

    //  生成 16 个 subkeys
    for (i=0; i<16; i++)
    {
        // 循环左移
        temp = des_rol(temp, SHIFT_TIMES[i]);

        // PC2 置换
        subkeys[i] = DES_PC2_TRANSFORM(temp);
    }
}

uint64_t des_crypt(uint64_t subkeys[16], uint64_t data, int mode)
{
    uint64_t left;
    uint64_t right;
    int      i, n;

    data = DES_IP_TRANSFORM(data);

    for (i=0; i<16; i++)
    {
        if (mode) n = 15 - i;
        else      n = i;

        right  = data & 0xffffffff;
        right  = DES_E_TRANSFORM(right);
        right ^= subkeys[n];
        right  = des_sbox(right);
        right  = DES_P_TRANSFORM(right);
        left   = data >> 32;
        left  ^= right;
        right  = data & 0xffffffff;
        if (i != 15) data = left  | (right << 32);
        else         data = right | (left  << 32);
    }

    data = DES_FP_TRANSFORM(data);
    return data;
}

static void str_to_uint64(uint64_t *value, char *str)
{
    int i, hex;

    *value = 0; // clear it first
    for (i=0; i<16; i++)
    {
        if      (str[i] >= '0' && str[i] <= '9') hex = str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'F') hex = str[i] - 'A' + 10;
        else if (str[i] >= 'a' && str[i] <= 'f') hex = str[i] - 'a' + 10;
        else hex = 0;

        *value <<= 4;
        *value  |= hex;
    }
}

static void uint64_to_str(char *str, uint64_t value)
{
    char hex;
    int  i;

    for (i=0; i<16; i++)
    {
        hex = value >> 60;
        if      (hex >= 0  && hex <= 9 ) str[i] = '0' + hex;
        else if (hex >= 10 && hex <= 15) str[i] = 'A' + hex - 10;
        value <<= 4;
    }
    str[i] = '\0';
}

//
// 参数说明：
// key  : 密钥
// src  : 源数据
// dst  : 目的数据
// mode : 0 - 表示加密，src 为明文，dst 为加密后的密文
//        1 - 表示解密，src 为密文，dst 为解密后的明文
// 
void str_des_crypt(char *key, char *src, char *dst, int mode)
{
    uint64_t u64key;
    uint64_t u64src;
    uint64_t u64dst;
    uint64_t subkeys[16];

    str_to_uint64(&u64key, key);
    str_to_uint64(&u64src, src);

    des_make_subkeys(u64key, subkeys);
    u64dst = des_crypt(subkeys, u64src, mode);
    uint64_to_str(dst, u64dst);
}

//
// 参数说明：
// key  : 密钥
// src  : 源文件名
// dst  : 目的文件名
// mode : 0 - 表示加密
//        1 - 表示解密
// 返回值 0 - 失败，1 - 成功
int file_des_crypt(uint64_t u64key, char *src, char *dst, int mode)
{
    uint64_t u64src;
    uint64_t u64dst;
    uint64_t subkeys[16];
    char    *pbufsrc = (char*)&u64src;
    FILE *fpsrc = NULL;
    FILE *fpdst = NULL;
    int   retv  = 0;
    int   curv  = 0;
    char  fill  = 0;
    char  space = ' ';

    // make sub keys
    des_make_subkeys(u64key, subkeys);

    fpsrc = fopen(src, "rb");
    fpdst = fopen(dst, "wb");
    if (!fpsrc || !fpdst) goto error_handler;

    while (1)
    {
        curv = fgetc(fpsrc);
        if (curv == EOF) break;
        else pbufsrc[0] = curv;

        curv      = fgetc(fpsrc);
        pbufsrc[1]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[2]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[3]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[4]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[5]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[6]= (curv != EOF) ? curv : space;

        curv      = fgetc(fpsrc);
        pbufsrc[7]= (curv != EOF) ? curv : space;

        u64dst = des_crypt(subkeys, u64src, mode);
        fwrite(&u64dst, sizeof(u64dst), 1, fpdst);
    }

    // success
    retv = 1;

error_handler:
    if (fpsrc) fclose(fpsrc);
    if (fpdst) fclose(fpdst);
    return retv;
}

#if 0
int main(void)
{
    uint64_t key   = 0;
    uint64_t start = 0;
    uint64_t end   = 0;
    uint64_t cur   = 0;
    uint64_t data  = 0;
    uint64_t subkeys[16];

    char  outf[256];
    char  skip[256];
    char  str [256];
    FILE *fp = NULL;

    fp = fopen("sngen.ini", "r");
    if (!fp) return 0;

    fscanf(fp, "%"SCNx64, &key  ); fgets(skip, 256, fp);
    fscanf(fp, "%"SCNx64, &start); fgets(skip, 256, fp);
    fscanf(fp, "%"SCNx64, &end  ); fgets(skip, 256, fp);
    fscanf(fp, "%s"     , &outf ); fgets(skip, 256, fp);
    fclose(fp);

    printf("key    = %"PRIX64"\n", key  );
    printf("start  = %"PRIX64"\n", start);
    printf("end    = %"PRIX64"\n", end  );
    printf("output = %s\n"       , outf );


    fp = fopen(outf, "w+");
    if (!fp) return 0;

    fprintf(fp, "key    = %"PRIX64"\n", key  );
    fprintf(fp, "start  = %"PRIX64"\n", start);
    fprintf(fp, "end    = %"PRIX64"\n", end  );
    fprintf(fp, "output = %s\n"       , outf );
    fprintf(fp, "\nsn list:\n\n");

    printf("generating sn list...\n");
    des_make_subkeys(key, subkeys);
    for (cur=start; cur<=end; cur++)
    {
        data = des_crypt(subkeys, cur, 0);
        sprintf(str, "%016"PRIX64 "  -  %016"PRIX64, cur, data);
        fprintf(fp, "%s\n", str);
    }
    printf("done.\n");
    fclose(fp);
    return 0;
}
#endif

#if 0
int main(void)
{
    uint64_t key  = 0x13B5F982857A1C07LL;
    uint64_t data = 0x2013010319230001LL;
    uint64_t subkeys[16];

#if 1
    printf("input key : "); scanf("%"SCNx64, &key );
    printf("input sn  : "); scanf("%"SCNx64, &data);

    des_make_subkeys(key, subkeys);
    data = des_crypt(subkeys, data, 1);
    printf("%016"PRIX64"\n", data);
    getch();
#else
    printf("key = %"PRIX64 "\n", key);
    printf("%"PRIX64 "\n", data);

    des_make_subkeys(key, subkeys);
    data = des_crypt(subkeys, data, 0);
    printf("%"PRIX64 "\n", data);

    data = des_crypt(subkeys, data, 1);
    printf("%"PRIX64 "\n", data);
#endif

    return 0;
}
#endif

#if 0
int main(void)
{
    char *key = "13B5F982857A1C07";
    char *src = "63C0109E115E76FC";
    char dst[17];

    str_des_crypt(key, src, dst, 1);
    printf("%s\n", dst);
    return 0;
}
#endif

#define DEBUG 1
int main(int argc, char *argv[])
{
    uint64_t key;
    char     str[64];
    int      mode;
    int      retv;
    int      i;

#if DEBUG
    printf("argc = %d\n", argc);
    printf("argv = ");
    for (i=0; i<argc; i++) printf("%s ", argv[i]);
    printf("\n\n");
#endif

    if (argc < 5)
    {
        printf("usage  - fdes [key] [mode] [infile] [outfile]\n");
        printf("key    : a key for des encrypt or decrypt\n");
        printf("mode   : des crypt mode, 1 - encrypt,  -1 - decrypt\n");
        printf("infile : input file name\n");
        printf("outfile: output file name\n");
        return;
    }

    // 自动补齐 key 为 16 位
    strcpy(str, argv[1]);
    strcat(str, "0000000000000000");
    str_to_uint64(&key, str);

    // 计算 mode 的值
    mode = (atoi(argv[2]) == -1) ? 0 : 1;

#if DEBUG
    printf("key  = %"PRIX64 "\n", key);
    printf("mode = %d\n", mode);
#endif

    retv = file_des_crypt(key, argv[3], argv[4], mode);
    printf("%d\n", retv);
}



