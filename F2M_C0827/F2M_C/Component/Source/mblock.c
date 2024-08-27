/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
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

