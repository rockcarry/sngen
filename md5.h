#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>

void* md5_init (void);
void  md5_free (void *ctxt);
void  md5_reset(void *ctxt);
void  md5_data (void *ctxt, uint8_t *data, int size);
void  md5_done (void *ctxt, uint8_t  md5[16]);

#endif
