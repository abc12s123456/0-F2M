#include "mblock.h"

#include "libc.h"


u32 MBlock_Write_SelfAlign(MBlock_Type *mb)
{
    u8 *p = (u8 *)mb->Pdata;
    u32 addr = mb->Addr;
    u32 gran_size = mb->Gran_Size;
    
    u32 ntmp, nbytes;
    
    nbytes = mb->Num;
    
    ntmp = addr % gran_size;
    if(ntmp)
    {
        /* 使地址粒度对齐 */
        addr -= ntmp;
        
        if(mb->Read_Gran(addr, mb->Buffer) == 0)  return 0;
        memcpy(mb->Buffer + ntmp, p, gran_size - ntmp);
        if(mb->Write_Gran(addr, mb->Buffer) == 0)  return 0;
        
        addr += gran_size;
        p += (gran_size - ntmp);
        nbytes -= (gran_size - ntmp);
    }
    
    ntmp = nbytes / gran_size;
    if((u32)p % gran_size)
    {
        /* 数据指针未粒度对齐 */
        while(ntmp--)
        {
            memcpy(mb->Buffer, p, gran_size);
            mb->Write_Gran(addr, mb->Buffer);
            addr += gran_size;
            p += gran_size;
        }
    }
    else
    {
        while(ntmp--)
        {
            mb->Write_Gran(addr, p);
            addr += gran_size;
            p += gran_size;
        }
    }
    
    ntmp = nbytes % gran_size;
    if(ntmp)
    {
        /* 写入剩余不足粒度大小的数据 */
        if(mb->Read_Gran(addr, mb->Buffer) == 0)  return 0;
        memcpy(mb->Buffer, p, ntmp);
        if(mb->Write_Gran(addr, mb->Buffer) == 0)  return 0;
    }
    
    return mb->Num;
}

u32 MBlock_Read_SelfAlign(MBlock_Type *mb)
{
    u8 *p = (u8 *)mb->Pdata;
    u32 addr = mb->Addr;
    u32 gran_size = mb->Gran_Size;
    
    u32 ntmp, nbytes;

    nbytes = mb->Num;
    
    ntmp = addr % gran_size;
    if(ntmp)
    {
        /* 使地址粒度对齐 */
        addr -= ntmp;
        if(mb->Read_Gran(addr / gran_size, mb->Buffer) == 0)  return 0;
        memcpy(p, mb->Buffer + ntmp, gran_size - ntmp);
        addr += gran_size;
        p += (gran_size - ntmp);
        nbytes -= (gran_size - ntmp);
    }
    
    ntmp = nbytes / gran_size;
    if((u32)p % gran_size)
    {
        /* 数据指针未粒度对齐 */
        while(ntmp--)
        {
            mb->Read_Gran(addr, mb->Buffer);
            memcpy(p, mb->Buffer, gran_size);
            addr += gran_size;
            p += gran_size;
        }
    }
    else
    {
        while(ntmp--)
        {
            mb->Read_Gran(addr, p);
            addr += gran_size;
            p += gran_size;
        }
    }
    
    ntmp = nbytes % gran_size;
    if(ntmp)
    {
        /* 读取剩余不足粒度大小的数据 */
        if(mb->Read_Gran(addr, mb->Buffer) == 0)  return 0;
        memcpy(p, mb->Buffer, gran_size - ntmp);
    }
    
    return mb->Num;
}

