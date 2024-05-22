#ifndef _MBLOCK_H_
#define _MBLOCK_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


typedef struct
{
    u32 Addr;
    void *Pdata;
    u32 Num;
    
    /* 粒度大小的缓存，至少是粒度数量字节对齐 */
    u8  *Buffer;
    
    /* 粒度大小，以底层允许读写的最小单元中的大者为准 */
    u32 Gran_Size;
    
    /* 粒度读写，由底层实现 */
    u32 (*Write_Gran)(u32 addr, const void *pdata);
    u32 (*Read_Gran)(u32 addr, void *pdata);
}MBlock_Type;


u32 MBlock_Write_SelfAlign(MBlock_Type *mb);
u32 MBlock_Read_SelfAlign(MBlock_Type *mb);


#ifdef __cplusplus
}
#endif

#endif

