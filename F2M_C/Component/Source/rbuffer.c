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
#include "rbuffer.h"
#include "mm.h"

#include "fw_debug.h"

#include "libc.h"


extern void FW_Lock(void);
extern void FW_Unlock(void);


/* 通过中断开关对RB_Buffer进行临界保护 */
#define RB_Lock              FW_Lock
#define RB_Unlock            FW_Unlock

#define MIN(num1, num2)      ((num1) < (num2) ? (num1) : (num2))


__INLINE_STATIC_ Bool RB_Data_IsCC(RB_Type *rb);

__INLINE_STATIC_ u32  RB_Set_Index(RB_Type *rb, u32 *index, u32 value);
__INLINE_STATIC_ u32  RB_Offset_Index(RB_Type *rb, u32 *index, u32 offset);

__INLINE_STATIC_ u32  RB_Get_Head(RB_Type *rb);
__INLINE_STATIC_ u32  RB_Set_Head(RB_Type *rb, u32 value);
__INLINE_STATIC_ u32  RB_Offset_Head(RB_Type *rb, u32 offset);


RB_Type *RB_Create(u32 size)
{
    RB_Type *rb = MM_Malloc(sizeof(RB_Type));
    
    if(rb != NULL)
    {
        rb->Buffer = MM_Malloc(size);
        if(rb->Buffer == NULL)  MM_Free(rb);
    }
    
    if(rb)  
    {
        rb->Size = size;
        
        rb->Head_Index = 0;
        rb->Tail_Index = 0;
        rb->Full_Flag = False;
        
        rb->Dynamic_Flag = True;
    }
    
    return rb;
}

/*  
备注：
*/
void RB_Delete(RB_Type *rb)
{
    if(rb->Dynamic_Flag == True)
    {
        MM_Free(rb->Buffer);
        MM_Free(rb);
    }
}

Bool RB_Init(RB_Type *rb, u32 size)
{
    FW_ASSERT(rb);
    
    if(size == 0)  return False;
    
    RB_Lock();
    
    if(rb->Buffer == NULL)
    {
        rb->Buffer = (u8 *)MM_Malloc(size);
        if(rb->Buffer == NULL)
        {
            RB_Unlock();
            return False;
        }
    }
    
    rb->Size = size;
    
    rb->Head_Index = 0;
    rb->Tail_Index = 0;
    rb->Full_Flag = False;
    rb->Dynamic_Flag = False;
    
    RB_Unlock();
    
    return True;
}

u8  *RB_Get_Buffer(RB_Type *rb)
{
    return rb->Buffer;
}

void RB_Clear_Buffer(RB_Type *rb)
{
    RB_Lock();
    rb->Head_Index = 0;
    rb->Tail_Index = 0;
    rb->Full_Flag = False;
    RB_Unlock();
}

void RB_Fill_Buffer(RB_Type *rb, u8 value)
{
    RB_Lock();
    rb->Head_Index = 0;
    rb->Tail_Index = 0;
    rb->Full_Flag = False;
    memset(rb->Buffer, value, rb->Size);
    RB_Unlock();
}

u32  RB_Get_BufferSize(RB_Type *rb)
{
    return rb->Size;
}

/*
备注：Head_Index与Tail_Index相等时，通过Full_Flag判断数据为空还是满
*/
u32  RB_Get_DataLength(RB_Type *rb)
{
    u32 tmp;
    
    if(rb->Full_Flag == True)
    {
        return rb->Size;
    }
    
    tmp = RB_Get_BufferSize(rb);
    if(tmp == 0)  return 0;
    
    RB_Lock();
    if(rb->Tail_Index >= rb->Head_Index)
    {
        tmp = rb->Tail_Index - rb->Head_Index;
    }
    else
    {
        tmp = rb->Size - rb->Head_Index + rb->Tail_Index;
    }
    RB_Unlock();
    
    return tmp;
}

u32  RB_Write(RB_Type *rb, const u8 *pdata, u32 num)
{
    u32 remain;
    s32 tmp;
    
    if(num == 0)  return 0;
    
    /* pdata与缓存地址重叠 */
    tmp = (u32)pdata - (u32)rb->Buffer;
    tmp = (tmp < 0) ? -tmp : tmp;
    if(tmp <= RB_Get_BufferSize(rb))  return 0;
    
    RB_Lock();
    
    /* 剩余的缓存空间不足以写入全部的pdata，会引起缓存溢出；溢出后，覆盖处理 */
    tmp = 0;
    if(RB_Get_DataLength(rb) + num >= rb->Size)
    {
        tmp = 1;
    }
    
    remain = rb->Size - rb->Tail_Index;
    if(remain >= num)
    {
        /* 当前周期内可以写入所有数据 */
        memcpy(rb->Buffer + rb->Tail_Index, pdata, num);
        rb->Tail_Index = (rb->Tail_Index + num) % rb->Size;
    }
    else
    {
        /* 数据需要作两部分写入：当前周期&循环到下一个周期 */
        memcpy(rb->Buffer + rb->Tail_Index, pdata, remain);
        memcpy(rb->Buffer, pdata + remain, num - remain);
        rb->Tail_Index = num - remain;
    }
    
    /* 头尾指针重合时有两种情况：1、缓存为空；2、缓存满；通过Length可以判断 */
    if(tmp)
    {
        RB_Set_Head(rb, rb->Tail_Index);         //将Head偏移到有效数据的起始位置
        rb->Full_Flag = True;
    }
    
    RB_Unlock();
    
    return num;
}

u32  RB_Read(RB_Type *rb, u8 *pdata, u32 num)
{
    u32 retain;
    s32 tmp;
    
    if(num == 0)  return 0;
    
    /* pdata与RB缓存地址重叠 */
    tmp = (u32)pdata - (u32)rb->Buffer;
    tmp = (tmp < 0) ? -tmp : tmp;
    if(tmp <= RB_Get_BufferSize(rb))  return 0;
    
    RB_Lock();
    
    /* 缓存中没有足够的数据 */
    if(RB_Get_DataLength(rb) < num)  
    {
        RB_Unlock();
        return 0;
    }
    
    retain = rb->Size - rb->Head_Index;
    if(retain >= num)
    {
        /* 有效数据在同一个周期中 */
        memcpy(pdata, rb->Buffer + rb->Head_Index, num);
        rb->Head_Index = (rb->Head_Index + num) % rb->Size;
    }
    else
    {
        /* 有效数据需要跨周期拷贝 */
        memcpy(pdata, rb->Buffer + rb->Head_Index, retain);
        memcpy(pdata + retain, rb->Buffer, num - retain);
        rb->Head_Index = num - retain;
    }
    
    rb->Full_Flag = False;
    
    RB_Unlock();
    
    return num;
}

u32  RB_Read_All(RB_Type *rb, u8 *pdata, u32 cntout)
{
    u32 num;
    RB_Lock();
    num = MIN(cntout, RB_Get_DataLength(rb));
    num = RB_Read(rb, pdata, num);
    RB_Unlock();
    return num;
}

/**
@功能: 目标字符串中查找指定字符串
@参数: dst, 目标字符串，可以不包含末尾的'\0'
       s, 指定字符串
       num, 目标字符串的长度
@返回: NULL, 目标字符串中不存在指定的字符串
       其它, 指定字符串在目标字符串中的起始地址
@备注：1、s虽然是以字符串的形式定义，但实际使用RB时，s中是可能含有'\0'的。所以在
          检索到*s++ == 0时，继续向下检索，直到检索完num个数据为止
*/
static char *Strfind(const char *dst, const char *s, u32 num)
{
    char *cp, *s1, *s2;
    
    if(*s == '\0')  return (char *)dst;
    if(num == 0)  return NULL;
    
    cp = (char *)dst;
    
    while(*cp)
    {
        s1 = cp;
        s2 = (char *)s;
        
        while(*s1 && *s2 && !(*s1 - *s2))
        {
            s1++;
            s2++;
        }
        
        if(*s2 == '\0')  return (cp);
        if((cp - dst) >= num - 1)  return NULL;
        cp++;
    }
    
    return NULL;
}

static int  Strcmp(const char *src, const char *dst, u32 num)
{
    int ret;
    u32 cnt;
    
    /* 需要比较的长度为0，则认为两组字符串相同 */
    if(num == 0)  return 0;
    
    ret = 0;
    
    while(!(*(u8 *)src - *(u8 *)dst))
    {
        ret = *(u8 *)src - *(u8 *)dst;
        if(ret != 0)  break;
        src++, dst++, cnt++;
        if(cnt >= num)  return 0;
    }
    
    return (int)(ret % 2);
}

/**
@功能: 判断临界部分是否包含指定的字符串
@参数: Type, FIFO对象
       buffer, Head指向的接收缓存位置
       s，指定的字符串
       length, 当前缓存数据长度
       tbuf, 临时缓存指针
       offset, 运算完成后Head_Index是否偏移。True: 偏移；False: 不偏移
@返回: NULL, 临界处不包含s
       s第一次出现的位置地址
@备注: 1、读取结果中包含首字符串
       2、tbuf.length >= Min((s.length - 1) * 2, length)
*/
static char *RB_Find_StringCS_RL1(RB_Type *rb,
                                  char *buffer,
                                  const char *s,
                                  u32 length,
                                  Bool offset)
{
    u32 slen = strlen(s);
    u32 len = length;
    u32 size = RB_Get_BufferSize(rb);
    
    u32 head = buffer - (char *)rb->Buffer;
    u32 len_1 = size - head;
    char *p = NULL, *s1, *s2;
    u32 i;
    int cp;
    
    /* head~size-1(第一部分)的长度大于slen */
    if(len_1 >= slen)
    {
        p = Strfind(buffer, s, len_1);
        if(p != NULL)
        {
            RB_Offset_Index(rb, &head, p - buffer);
            if(offset)  rb->Head_Index = head;
            
            /* 无论Head_Index是否偏移，返回的有效地址都是head指向的位置 */
            return (char *)&rb->Buffer[head];
        }
        else  /* head~size-1数据中不包含s */
        {
            /* head指向下一步需要判断第一个位置 */
            RB_Set_Index(rb, &head, size - slen + 1);
            
            /* 缓存剩余的有效数据长度 */
            len = len - len_1 + (slen - 1);

            /* 第一部分剩余的数据长度 */
            len_1 = slen - 1;
        }
    }
    
    /* head~size-1的数据不足slen或不包含s */
    if(p == NULL)
    {
        s1 = (char *)s;
        for(i = 0; i < slen - 1; i++)
        {
            cp = Strcmp((char *)rb->Buffer - i, s1, i + 1);
            if(cp == 0)
            {
                s2 = (char *)s + 1 + i;
                cp = Strcmp((char *)rb->Buffer, s2, slen - i);
                if(cp == 0)  
                {
                    /* 指定字符串比对成功 */
                    p = (char *)rb->Buffer - i;
                    break;
                }
            }
        }
        
        if(p)
        {
            RB_Offset_Index(rb, &head, p - buffer);
        }
        else
        {
            /* 临界处不包含s，在缓存下半周的数据中继续查找 */
            RB_Set_Index(rb, &head, 0);
            p = Strfind((char *)rb->Buffer, s, len - len_1);
            if(p != NULL)
            {
                RB_Offset_Index(rb, &head, p - (char *)rb->Buffer);
            }
            else  /* 缓存数据中不包含s */
            {
                RB_Offset_Index(rb, &head, len - len_1);
                if(offset)  rb->Head_Index = head;
                return NULL;
            }
        }
    }
    
    if(offset)  rb->Head_Index = head;
    
    return (char *)&rb->Buffer[head];
}

static char *RB_Find_StringCS(RB_Type *rb, 
                              char *buffer, 
                              const char *s, 
                              u32 length, 
                              void *tbuf, 
                              Bool offset)
{
    u32 slen = strlen(s);
    u32 len = length;
    u32 size = RB_Get_BufferSize(rb);
    
    u32 head = buffer - (char *)rb->Buffer;
    u32 len_1 = size - head;
    u32 len_2;
    char *p = NULL;
    
    /* head~size-1(第一部分)的长度大于slen */
    if(len_1 >= slen)
    {
        p = Strfind(buffer, s, len_1);
        if(p != NULL)
        {
            RB_Offset_Index(rb, &head, p - buffer);
            if(offset)  rb->Head_Index = head;
            
            /* 无论Head_Index是否偏移，返回的有效地址都是head指向的位置 */
            return (char *)&rb->Buffer[head];
        }
        else  /* head~size-1数据中不包含s */
        {
            /* head指向下一步需要判断第一个位置 */
            RB_Set_Index(rb, &head, size - slen + 1);
            
            /* 缓存剩余的有效数据长度 */
            len = len - len_1 + (slen - 1);  

            /* 第一部分剩余的数据长度 */
            len_1 = slen - 1;                          
        }
    }
    
    /* head~size-1的数据不足slen或不包含s */
    if(p == NULL)
    {
        /* 通过临时缓存将[size - len_1, size - 1]及[0, len_1]的数据规整 */
        memcpy(tbuf, (char *)&rb->Buffer[head], len_1);
        len_2 = MIN((slen - 1) * 2, len) - len_1;
        memcpy((char *)tbuf + len_1, rb->Buffer, len_2);
        
        /* tbuffer.Length >= Min((slen - 1) * 2, len) */
        p = Strfind((char *)tbuf, s, len_1 + len_2);
        if(p != NULL)
        {
            RB_Offset_Index(rb, &head, p - (char *)tbuf);
        }
        else
        {
            /* 临界处不包含s，在缓存下半周的数据中继续查找 */
            RB_Set_Index(rb, &head, 0);
            p = Strfind((char *)rb->Buffer, s, len - len_1);
            if(p != NULL)
            {
                RB_Offset_Index(rb, &head, p - (char *)rb->Buffer);
            }
            else  /* 缓存数据中不包含s */
            {
                RB_Offset_Index(rb, &head, len - len_1);
                if(offset)  rb->Head_Index = head;
                return NULL;
            }
        }
    }
    
    if(offset)  rb->Head_Index = head;
    
    return (char *)&rb->Buffer[head];
}
/**/

u32  RB_Read_To(RB_Type *rb, const char *s, u8 *pdata, u32 cntout)
{
    u32 slen, len;
    u32 size;
    char *buffer;
    char *p;
    char *pMin, *pMax;
    
    RB_Lock();
    
    slen = strlen(s);
    len = RB_Get_DataLength(rb);
    if(len < slen)
    {
        RB_Unlock();
        return 0;
    }
    
    size = RB_Get_BufferSize(rb);
    buffer = (char *)&rb->Buffer[rb->Head_Index];
    p = NULL;
    pMin = (char *)rb->Buffer;
    pMax = (char *)&rb->Buffer[size - 1];
    
    if(RB_Data_IsCC(rb) == True)
    {
        p = RB_Find_StringCS(rb, buffer, s, len, pdata, False);
        if(p == NULL)  
        {
            RB_Unlock();
            return 0;
        }
            
        if(p > pMax || p < pMin)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
        
        if(p >= buffer)
        {
            len = p - buffer + slen;
        }
        else
        {
            len = p + size - buffer + slen;
        }
    }
    else
    {
        p = Strfind(buffer, s, len);
        if(p == NULL)  
        {
            RB_Unlock();
            return 0;
        }
        
        if(p > pMax || p < pMin)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
        
        len = p - buffer + slen;
    }
    
    /* 可读的数据长度已超出指定的长度，则只读取指定的长度 */
    if(len > cntout)
    {
        size = len - cntout;
        len = cntout;
    }
    
    len = RB_Read(rb, pdata, len);
    
    /* 头指针偏移至指定字符串所在位置的下一个地址，否则会重复读取 */
    RB_Offset_Head(rb, size);
    
    RB_Unlock();
    
    return len;
}

u32  RB_Read_Line(RB_Type *rb, u8 *pdata, u32 cntout)
{
    return RB_Read_To(rb, "\r\n", pdata, cntout);
}

u32  RB_Read_Inter(RB_Type *rb, const char *s1, const char *s2, u8 *pdata, u32 cntout)
{    
    u32 slen1, slen2, len;
    u32 size;
    char *buffer;
    char *p1, *p2;
    char *pMin, *pMax;
    
    RB_Lock();
    
    slen1 = strlen(s1);
    slen2 = strlen(s2);
    len = RB_Get_DataLength(rb);
    if(len < slen1 + slen2)  
    {
        RB_Unlock();
        return 0;                                                               //缓存中没有足够的数据
    }
    
    size = RB_Get_BufferSize(rb);
    buffer = (char *)&rb->Buffer[rb->Head_Index];
    p1 = NULL, p2 = NULL;
    pMin = (char *)rb->Buffer; 
    pMax = (char *)&rb->Buffer[size - 1];
    
    if(RB_Data_IsCC(rb) == True)                                        //缓存数据跨周期
    {
        p1 = RB_Find_StringCS(rb, buffer, s1, len, pdata, True);
        
        /* 缓存中不包含s1，直接退出 */
        if(p1 == NULL)
        {
            RB_Unlock();
            return 0;
        }
        
        /* 缓存污染 */
        if(p1 > pMax || p1 < pMin)   
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
        
        if(p1 >= buffer)                                                        //s1还在第一个周期
        {
            len = len - (p1 - buffer) - slen1;                                  //判断s2时的缓存数据长度
            
            if(pMax - p1 > slen1)                                               //第一个数据周期还有足够长的数据
            {
                p2 = RB_Find_StringCS(rb, p1 + slen1, s2, len, pdata, False);
            }
            else
            {
                p2 = Strfind((char *)rb->Buffer + (slen1 - (pMax - p1)), s2, len);
            }
            
            if(p2 == NULL)
            {
                RB_Unlock();
                return 0;
            }
            
            if(p2 > pMax || p2 < pMin)
            {
                RB_Clear_Buffer(rb);
                RB_Unlock();
                return 0;
            }
        }
        else                                           
        {
            /* s1已经在第二个周期 */
            len = len - (p1 + size - buffer) - slen1;
            
            p2 = Strfind(p1 + slen1, s2, len);
            if(p2 == NULL)
            {
                RB_Unlock();
                return 0;
            }
            
            if(p2 > pMax)
            {
                RB_Clear_Buffer(rb);
                RB_Unlock();
                return 0;
            }
        }
    }
    else
    {
        p1 = Strfind(buffer, s1, len);
        if(p1 == NULL)  
        {
            RB_Unlock();
            return 0;
        }
        
        if(p1 > pMax)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
        
        RB_Offset_Head(rb, p1 - buffer);
        
        p2 = Strfind(p1 + slen1, s2, len - slen1);
        if(p2 == NULL)  
        {
            RB_Unlock();
            return 0;
        }
        
        if(p2 > pMax)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
    }
    
    if(p1 <= p2)
    {
        len = p2 - p1 + slen2;
    }
    else
    {
        len = p2 + size - p1 + slen2;
    }
    
    if(len > cntout)                      
    {
        /* 区间读取时，数据超出指定长度，丢弃并清缓存 */
        RB_Clear_Buffer(rb);
        RB_Unlock();
        return 0;
    }
    
    len = RB_Read(rb, pdata, len);
    
    RB_Unlock();
    
    return len;
}

u32  RB_Read_Fisrt(RB_Type *rb, const char *s, u8 *pdata, u32 num)
{
    u32 slen = strlen(s), len;
    u32 size;
    char *buffer;
    char *p, *pMin, *pMax;
    
    RB_Lock();
    
    slen = strlen(s);
    len = RB_Get_DataLength(rb);
    if(len < slen + num)  
    {
        RB_Unlock();
        return 0;                           //缓存中没有足够的数据
    }
    
    size = RB_Get_BufferSize(rb);
    buffer = (char *)&rb->Buffer[rb->Head_Index];  //缓存起始位置
    p = NULL;
    pMin = (char *)rb->Buffer;
    pMax = (char *)&rb->Buffer[size - 1];
    
    if(RB_Data_IsCC(rb) == True)
    {
        p = RB_Find_StringCS(rb, buffer, s, len, pdata, True);
        if(p == NULL)  
        {
            RB_Unlock();
            return 0;
        }
        
        if(p > pMax || p < pMin)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return 0;
        }
    }
    else
    {
        p = Strfind(buffer, s, len);
        if(p != NULL)
        {
            RB_Offset_Head(rb, p - buffer);    //head指向首字符开始的位置
        }
        else
        {
            RB_Offset_Head(rb, len);           //缓存数据中不存在s
            RB_Unlock();
            return 0;
        }
    }
        
    if(RB_Get_DataLength(rb) < slen + num)  
    {
        RB_Unlock();
        return 0;
    }
    
    len = RB_Read(rb, pdata, slen + num);      //返回结果中保留s
    
    RB_Unlock();
    
    return len;
}

/**
@功能: 以Head + Length的方式从RB中读取数据
@参数: rb, RB对象
       head，首字符
       length_site, 帧数据长度在数据包中的位置(从0开始计算)
       other_num, 不在Data_Length计数范围内的数据数量
       pdata, 读取数据指针
       cntout, 超出计数，cntout <= sizeof(pdata), 防止内存溢出
@返回: 读取的数据长度，以字节为单位
@备注: @other_num, 根据具体的数据帧确定。例如：A5 04 11 12 13 14 sum
        head = A5, length_site = 1, other_num = 3(A5 04 sum)
       @pdata内存空间≥cntout
@记录：2020-12-29，修改，对于数据异常时的情况，不再进行清缓存处理，否则当数据接收过快时，会导致查询的
                         接收长度始终不足，造成数据无法正常读取
2024-05-27, 修复，对于接收的数据长度，最小值为other_num
*/
u32  RB_Read_HL(RB_Type *rb, const u8 head, u8 length_site, u8 other_num, u8 *pdata, u32 cntout)
{
    u32 len;
    u32 head_index;
    u32 packet_len;
    
    RB_Lock();
    
    len = RB_Get_DataLength(rb);
    
    if(len < other_num)  
    {
        RB_Unlock();
        return 0;                                     //接收数据不足
    }
    
    while(rb->Buffer[rb->Head_Index] != head)         //先获取head
    {
        if(--len < length_site + other_num)  
        {
            RB_Unlock();
            return 0;                                 //剩余数据不足
        }
        RB_Offset_Head(rb, 1);
    }
    
    head_index = RB_Get_Head(rb);
    packet_len = rb->Buffer[RB_Offset_Index(rb, &head_index, length_site)];
    if(packet_len + other_num > cntout)               //数据异常
    {
        RB_Unlock();
        return 0;                                     //数据接收过快或读取不及时，造成数据堆积，导致异常
    }
    
    if(len < packet_len + other_num)  
    {
        RB_Unlock();
        return 0;
    }
    
    len = RB_Read(rb, pdata, packet_len + other_num);
    
    RB_Unlock();
    
    return len;
}


char *RB_Find_String(RB_Type *rb, const char *s)
{
    return Strfind((char *)rb->Buffer, s, rb->Size);
}

Bool RB_String_IsExist(RB_Type *rb, const char *s)
{    
    u32 slen, len;
    u32 size;
    char *buffer;
    char *p, *pMin, *pMax;
    
    RB_Lock();
    
    slen = strlen(s);
    len = RB_Get_DataLength(rb);
    if(len < slen)  
    {
        RB_Unlock();
        return False;                                         //缓存中没有足够数据
    }
    
    size = RB_Get_BufferSize(rb);
    buffer = (char *)&rb->Buffer[rb->Head_Index];             //缓存起始位置
    p = NULL;
    pMin = (char *)rb->Buffer;
    pMax = (char *)&rb->Buffer[size - 1];
    
    if(RB_Data_IsCC(rb) == True)
    {
        p = RB_Find_StringCS_RL1(rb, buffer, s, len, False);
        if(p == NULL)
        {
            RB_Unlock();
            return False;
        }
        
        if(p > pMax || p < pMin)
        {
            RB_Clear_Buffer(rb);
            RB_Unlock();
            return False;
        }
    }
    else
    {
        p = Strfind(buffer, s, len);
        if(p == NULL)
        {
            RB_Unlock();
            return False;
        }
    }
    
    RB_Unlock();
    
    return True;
}

__INLINE_STATIC_ Bool RB_Data_IsCC(RB_Type *rb)
{
    Bool state;
    RB_Lock();
    state = (rb->Tail_Index < rb->Head_Index) ? True : False;
    RB_Unlock();
    return state;
}

__INLINE_STATIC_ u32  RB_Set_Index(RB_Type *rb, u32 *index, u32 value)
{
    *index = (value < rb->Size) ? value : (value % rb->Size);
    return *index;
}

__INLINE_STATIC_ u32  RB_Offset_Index(RB_Type *rb, u32 *index, u32 offset)
{
    *index = (*index + offset) % rb->Size;
    return *index;
}

__INLINE_STATIC_ u32  RB_Get_Head(RB_Type *rb)
{
    return rb->Head_Index;
}

__INLINE_STATIC_ u32  RB_Set_Head(RB_Type *rb, u32 value)
{
    rb->Head_Index = (value < rb->Size) ? value : (value % rb->Size);
    return rb->Head_Index;
}

__INLINE_STATIC_ u32  RB_Offset_Head(RB_Type *rb, u32 offset)
{
    rb->Head_Index = (rb->Head_Index + offset) % rb->Size;
    return rb->Head_Index;
}














