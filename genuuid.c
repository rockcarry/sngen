#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *help = "\n"
"  genuuid v1.0.2-20151222 by ck            \n\n"
"  usage: genuuid seed prefix len start num \n"
"         seed   - random seed number       \n"
"         prefix - uuid prefix string       \n"
"         len    - uuid lenght              \n"
"         start  - start uuid index         \n"
"         number - total uuid number        \n\n"
"  example:                                 \n"
"    genuuid 888 AJ 18 0 1000               \n\n"
"  RAND_MAX:                                \n"
"    %d                                     \n\n"
;

#pragma pack(1)
typedef struct {
    uint16_t a;
    uint16_t b;
    uint16_t c;
    uint16_t d;
    uint16_t e;
    uint16_t f;
    uint16_t g;
    uint16_t h;
} UUID;
#pragma pack()

typedef struct {
    UUID uuid;
    int  next;
} HASH_ITEM;

#define HASH_TABLE_MAX_SIZE  (1024 * 1024) // size must be 2^n

typedef struct {
    HASH_ITEM items[HASH_TABLE_MAX_SIZE];
    int       tablesize;
    int       availpos;
} HASH_TABLE;

static HASH_TABLE g_uuid_hash_table = {0};

//++ hash function used by php
static uint32_t hashpjw(char *arKey, unsigned int nKeyLength)
{
    unsigned int h = 0, g;
    char *arEnd = arKey + nKeyLength; 

    while (arKey < arEnd) {
        h = (h << 4) + *arKey++;
        if ((g = (h & 0xF0000000))) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }

    return h;
}
//-- hash function used by php

static void hash_table_init(HASH_TABLE *table, int size)
{
    int i;

    //++ for table size
    if (  size > HASH_TABLE_MAX_SIZE
       || size == 0 ) {
        table->tablesize = HASH_TABLE_MAX_SIZE;
    }
    else {
        table->tablesize = size;
    }
    //-- for table size

    //++ for next of chain
    for (i=0; i<table->tablesize; i++) {
        table->items[i].next = -1;
    }
    //-- for next of chain

    // for availpos
    table->availpos = 0;
}

static int hash_table_add(HASH_TABLE *table, UUID *uuid)
{
    int pos  = hashpjw((char*)uuid, sizeof(UUID)) % table->tablesize;
    int next = table->items[pos].next;

    /*
        next
            == -1 mean item is free for use
            == -2 mean item is used, and is the tail item of chain
            >=  0 mean the next item pos of chain
     */
    if (next == -1) {
        //++ first item is not used, so put uuid into it, and mark it used as tail of chain
        memcpy(&(table->items[pos].uuid), uuid, sizeof(UUID));
        table->items[pos].next = -2;
        //-- first item is not used, so put uuid into it, and mark it used as tail of chain
    }
    else {
        //++ find in chain, to check if this uuid already used or not
        while (1) {
            if (memcmp(uuid, &(table->items[pos].uuid), sizeof(UUID)) == 0) {
                return -1; // is uuid already used return -1 directly
            }
            //+ check it reach tail of chain or not
            if (next == -2) {
                break;
            }
            //- check it reach tail of chain or not

            //+ move to next item of chain
            pos  = next;
            next = table->items[pos].next;
            //- move to next item of chain
        }
        //-- find in chain, to check if this uuid already used or not

        //++ find a free item from availpos to end of hash table
        while (table->availpos < HASH_TABLE_MAX_SIZE) {
            if (table->items[table->availpos].next == -1) {
                break;
            }
            else {
                table->availpos++;
            }
        }
        //-- find a free item from availpos to end of hash table

        //++ fill this free item with new uuid
        table->items[pos].next = table->availpos; // link this item with parent
        memcpy(&(table->items[table->availpos].uuid), uuid, sizeof(UUID));
        table->items[table->availpos].next = -2; // mark as tail of chain
        //-- fill this free item with new uuid

        table->availpos++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int  seed, len, start, num;
    int  valid;
    int  i;
    char prefix [16];
    char uuidstr[64];
    FILE *fp = NULL;

    //++ handle command line
    if (argc < 6) {
        printf(help, RAND_MAX);
        return 0;
    }

    seed = atoi(argv[1]);
    len  = atoi(argv[3]);
    start= atoi(argv[4]);
    num  = atoi(argv[5]);
    if (strcmp(argv[2], "null") == 0) {
        strcpy(prefix, "");
    }
    else {
        strcpy(prefix, argv[2]);
    }
    //-- handle command line

    // set rand seed
    srand(seed);

    // init hash table
    hash_table_init(&g_uuid_hash_table, HASH_TABLE_MAX_SIZE);

    //++ generate uuids
    printf("please wait:\n");
    for (i=0; i<g_uuid_hash_table.tablesize;) {
        UUID uuid;
        uuid.a = rand();
        uuid.b = rand();
        uuid.c = rand();
        uuid.d = rand();
        uuid.e = rand();
        uuid.f = rand();
        uuid.g = rand();
        uuid.h = rand();
        if (hash_table_add(&g_uuid_hash_table, &uuid) >= 0) {
            i++;
        }
        if (i % 1000 == 0) {
            printf(".");
        }
    }
    printf("\n");
    //-- generate uuids

    //++ output result
    fp = fopen("UUIDList.txt", "wb");
    if (fp) {
        for (i=0,valid=0; i<HASH_TABLE_MAX_SIZE && valid<start+num; i++) {
            if (g_uuid_hash_table.items[i].next != -1) {
                if (valid++ >= start) {
                    sprintf(uuidstr, "%s%04X%04X%04X%04X%04X%04X%04X%04X", prefix,
                        g_uuid_hash_table.items[i].uuid.a,
                        g_uuid_hash_table.items[i].uuid.b,
                        g_uuid_hash_table.items[i].uuid.c,
                        g_uuid_hash_table.items[i].uuid.d,
                        g_uuid_hash_table.items[i].uuid.e,
                        g_uuid_hash_table.items[i].uuid.f,
                        g_uuid_hash_table.items[i].uuid.g,
                        g_uuid_hash_table.items[i].uuid.h);
                    uuidstr[len] = '\0';
                    strcat(uuidstr, "\r\n");
                    fwrite(uuidstr, strlen(uuidstr), 1, fp);
                }
            }
        }
        fclose(fp);
    }
    //-- output result

    printf("done.\n");
    return 0;
}






