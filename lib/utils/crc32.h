#ifndef __CRC32_H_cfa26d9a55a54a3ca1b13fc349818689__
#define __CRC32_H_cfa26d9a55a54a3ca1b13fc349818689__
/******************************************************************************/
#include "utils/type.h"

/*
* https://github.com/ETrun/crc32
*/
extern uint32 
__crc32(uint32 crc_in, const void *buf, uint32 size);

extern uint32
os_crc32(const void *buf, uint32 size);

#define OS_CRC32_STRINGLEN      8

/******************************************************************************/
#endif /* __CRC32_H_cfa26d9a55a54a3ca1b13fc349818689__ */
