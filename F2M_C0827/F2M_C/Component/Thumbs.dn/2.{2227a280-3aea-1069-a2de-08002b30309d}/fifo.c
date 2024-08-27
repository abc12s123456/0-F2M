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
 
#include "fifo.h"
#include "mm.h"

#include "fw_system.h"
#include "fw_debug.h"

#include "lib_string.h"

#include "libc.h"


/* 通过中断开关对FIFO进行临界保护 */
#define FIFO_Lock                 FW_Lock
#define FIFO_Unlock               FW_Unlock


#define MIN(num1, num2)           (((num1) < (num2)) ? (num1) : (num2))


//__INLINE_STATIC_ void FIFO_Set_DataLength(FIFO_Type *fifo, u32 length);

__INLINE_STATIC_ Bool FIFO_Data_IsCC(FIFO_Type *fifo);

__INLINE_STATIC_ u32  FIFO_Get_Head(FIFO_Type *fifo);
__INLINE_STATIC_ u32  FIFO_Set_Head(FIFO_Type *fifo, u32 value);
__INLINE_STATIC_ u32  FIFO_Offset_Head(FIFO_Type *fifo, u32 offset);


/**
@功能: FIFO初始化
@参数: fifo, FIFO对象
       block_num, 内存块数量
       block_size, 内存块大小
@返回: True, FIFO初始化成功
       False, FIFO初始化失败
@备注: @支持用户自定义内存
*/
Bool FIFO_Init(FIFO_Type *fifo, u32 block_num, u32 block_size)
{
    FW_ASSERT(fifo);
    
    FIFO_Lock();
    
    if(fifo->Buffer == NULL)
    {
        fifo->Buffer = (u8 *)MM_Malloc(block_num * block_size);
        if(fifo->Buffer == NULL)  
        {
            FIFO_Unlock();
            return False;
        }
    }
    
    fifo->Block_Num = block_num;
    fifo->Block_Size = block_size;
    fifo->Head_Index = 0;
    fifo->Tail_Index = 0;
    fifo->Full_Flag = False;
    
    FIFO_Unlock();
    
    FW_ASSERT(fifo->Buffer);
    FW_ASSERT(fifo->Block_Num);
    FW_ASSERT(fifo->Block_Size);
    
    return True;
}
/**/


void FIFO_Set_BlockSize(FIFO_Type *fifo, u32 size)
{
    fifo->Block_Size = size;
}

/**
@功能: 获取FIFO的数据块大小
@参数: Type, FIFO对象
@返回: FIFO的数据块大小，单位byte
@备注: 无
*/
u32 FIFO_Get_BlockSize(FIFO_Type *fifo)
{
    return fifo->Block_Size;
}
/**/


void FIFO_Set_BlockNum(FIFO_Type *fifo, u32 num)
{
    fifo->Block_Num = num;
}

/**
@功能: 获取FIFO的数据块数量
@参数: Type, FIFO对象
@返回: FIFO的数据块数量
@备注: 无
*/
u32 FIFO_Get_BlockNum(FIFO_Type *fifo)
{
    return fifo->Block_Num;
}
/**/

void FIFO_Set_Buffer(FIFO_Type *fifo, u8 *buffer)
{
    fifo->Buffer = buffer;
}

/**
@功能: 获取FIFO的缓存地址
@参数: Type, FIFO对象
@返回: FIFO的缓存起始地址
@备注: 无
*/
u8 *FIFO_Get_Buffer(FIFO_Type *fifo)
{
    return fifo->Buffer;
}
/**/

/**
@功能: 获取FIFO的大小
@参数: Type, FIFO对象
@返回: FIFO的大小，单位byte
@备注: 无
*/
u32 FIFO_Get_BufferSize(FIFO_Type *fifo)
{
    return (u32)(fifo->Block_Num * fifo->Block_Size);
}
/**/

/**
@功能: 获取FIFO中的数据数量
@参数: Type, FIFO对象
@返回: FIFO中的数据长度，单位block_size * 1byte
@备注: 无
*/
u32 FIFO_Get_DataLength(FIFO_Type *fifo)
{
    u32 tmp;
    
    if(fifo->Full_Flag == True)
    {
        return fifo->Block_Num;
    }
    
    tmp = FIFO_Get_BufferSize(fifo);
    if(tmp == 0)  return 0;
    
    FIFO_Lock();
    if(fifo->Tail_Index >= fifo->Head_Index)
    {
        tmp = fifo->Tail_Index - fifo->Head_Index;
    }
    else
    {
        tmp = fifo->Block_Num - fifo->Head_Index + fifo->Tail_Index;
    }
    FIFO_Unlock();
    
    return tmp;
}
/**/

u32 FIFO_Write(FIFO_Type *fifo, const void *pdata, u32 num)
{
    u32 remain;
    s32 tmp;
    
    if(num == 0)  return 0;
    
    /* pdata与FIFO缓存地址重叠 */
    tmp = (u32)pdata - (u32)fifo->Buffer;
    tmp = (tmp < 0) ? -tmp : tmp;
    if(tmp <= FIFO_Get_BufferSize(fifo))  return 0;
    
    FIFO_Lock();
    
    /* 剩余的缓存空间不足以写入全部的pdata，会引起缓存溢出；溢出后，覆盖处理 */
    tmp = 0;
    if(FIFO_Get_DataLength(fifo) + num >= fifo->Block_Num)
    {
        tmp = 1;
    }
    
    remain = fifo->Block_Num - fifo->Tail_Index;
    if(remain >= num)
    {
        /* 当前周期内可以写入所有数据 */
        memcpy((u8 *)fifo->Buffer + fifo->Tail_Index * fifo->Block_Size, 
               pdata, 
               num * fifo->Block_Size);
        fifo->Tail_Index = (fifo->Tail_Index + num) % fifo->Block_Num;
    }
    else
    {
        /* 数据需要作两部分写入：当前周期&循环到下一个周期 */
        memcpy((u8 *)fifo->Buffer + fifo->Tail_Index * fifo->Block_Size, 
               pdata, 
               remain * fifo->Block_Size);
        memcpy((u8 *)fifo->Buffer, 
               (u8 *)pdata + remain * fifo->Block_Size, 
               (num - remain) * fifo->Block_Size);
        fifo->Tail_Index = num - remain;
    }
    
    /* 头尾指针重合时有两种情况：1、缓存为空；2、缓存满；通过Length可以判断 */
    if(tmp)
    {
        FIFO_Set_Head(fifo, fifo->Tail_Index);
        fifo->Full_Flag = True;
    }
//    {
//        FIFO_Set_DataLength(fifo, fifo->Block_Num);                             //缓存已满
//        FIFO_Set_Head(fifo, fifo->Tail_Index);                                  //将Head偏移到有效数据的起始位置
//    }
//    else
//    {
//        /* 更新数据长度 */
//        FIFO_Set_DataLength(fifo, fifo->Data_Length + num);
//    }
    
    FIFO_Unlock();
    
    return num;
}

/**
@功能: 从FIFO中读取指定长度的数据
@参数: Type, FIFO对象
       pdata, 读取数据
       num, 读取数据的长度
@返回: 读取的数据数量，block_size * 1byte
@备注: 1、pdata内存空间≥num
       3、pdata地址与缓存地址不能出现重叠
@修改：2020-11-11，优化，增加了读取缓存与FIFO缓存地址一致时的处理
*/
u32  FIFO_Read(FIFO_Type *fifo, void *pdata, u32 num)
{
    u32 retain;
    s32 tmp;
    
    if(num == 0)  return 0;
    
    /* pdata与FIFO缓存地址重叠 */
    tmp = (u32)pdata - (u32)fifo->Buffer;
    tmp = (tmp < 0) ? -tmp : tmp;
    if(tmp <= FIFO_Get_BufferSize(fifo))  return 0;
    
    FIFO_Lock();
    
    /* 缓存中没有足够的数据 */
    if(FIFO_Get_DataLength(fifo) < num)  
    {
        FIFO_Unlock();
        return 0;
    }
    
    retain = fifo->Block_Num - fifo->Head_Index;
    
    /* retain >= num说明数据在同一个周期中，否则需要进行不同周期的拷贝 */
    if(retain >= num)
    {
        memcpy(pdata, 
               (u8 *)fifo->Buffer + fifo->Head_Index * fifo->Block_Size, 
               num * fifo->Block_Size);
        fifo->Head_Index = (fifo->Head_Index + num) % fifo->Block_Num;
    }
    else
    {
        memcpy(pdata, 
               (u8 *)fifo->Buffer + fifo->Head_Index * fifo->Block_Size, 
               retain * fifo->Block_Size);
        memcpy((u8 *)pdata + retain * fifo->Block_Size, 
               fifo->Buffer, 
               (num - retain) * fifo->Block_Size);
        fifo->Head_Index = num - retain;
    }
    
//    FIFO_Set_DataLength(fifo, fifo->Data_Length - num);
    fifo->Full_Flag = False;
    
    FIFO_Unlock();
    
    return num;
}
/**/

/**
@功能: 读取FIFO中的所有数据
@参数: Type, FIFO对象
       pdata, 读取数据
       cntout, 超出计数，cntout <= sizeof(pdata), 防止内存溢出
@返回: 读取的数据长度，单位block_size * 1byte
@备注: @pdata内存空间≥min(cntout, Buffer_Length)
@修改：2020-11-11，优化，增加了缓存溢出处理
*/
u32  FIFO_Read_All(FIFO_Type *fifo, void *pdata, u32 cntout)
{
    u32 num;
    FIFO_Lock();
    num = MIN(cntout, FIFO_Get_DataLength(fifo));
    num = FIFO_Read(fifo, pdata, num);
    FIFO_Unlock();
    return num;
}
/**/

//__INLINE_STATIC_ void FIFO_Set_DataLength(FIFO_Type *fifo, u32 length)
//{
//    fifo->Data_Length = length;
//}
///**/

/**
@功能: 判断缓存数据是否跨周期(cross cycle)
@参数: 无
@返回: True, 跨周期
       False, 未跨周期
@备注: 无
*/
__INLINE_STATIC_ Bool FIFO_Data_IsCC(FIFO_Type *fifo)
{
    Bool state;
    FIFO_Lock();
    state = (fifo->Tail_Index < fifo->Head_Index) ? True : False;
    FIFO_Unlock();
    return state;
}
/**/

/**
@功能: Index设置
@参数: index, 需要设置的索引
       value, 设置值
@返回: 设置后的Index
@备注: 无
*/
__INLINE_STATIC_ u32 FIFO_Set_Index(FIFO_Type *fifo, u32 *index, u32 value)
{
    *index = (value < fifo->Block_Num) ? value : (value % fifo->Block_Num);
    return *index;
}
/**/

/**
@功能: Index偏移
@参数: index, 需要进行偏移的索引
       offset, 偏移值
@返回: 偏移后的Index
@备注: 无
*/
__INLINE_STATIC_ u32 FIFO_Offset_Index(FIFO_Type *fifo, u32 *index, u32 offset)
{
    *index = (*index + offset) % fifo->Block_Num;
    return *index;
}
/**/

/**
@功能: 判断临界部分是否包含指定的字符串
@参数: Type, FIFO对象
       buffer, Head指向的接收缓存位置
       s，指定的字符串
       length, 当前缓存数据长度
       tbuffer, 临时缓存指针
       offset, 运算完成后Head_Index是否偏移。True: 偏移；False: 不偏移
@返回: NULL, 临界处不包含s
       s第一次出现的位置地址
@备注: @该函数及调用该函数的函数，仅针对block_size = 1的情况
       @读取结果中包含首字符串
       @pdata内存空间≥MIN(2 * (strlen(s) - 1), Get_Length())
*/
static char *FIFO_Find_StringCS(FIFO_Type *fifo, 
                              char *buffer, 
                              const char *s, 
                              u32 length, 
                              void *tbuffer, 
                              Bool offset)
{
    u32 slen = strlen(s);
    u32 len = length;
    u32 size = FIFO_Get_BufferSize(fifo);
    
    u32 head = buffer - (char *)fifo->Buffer;
    u32 len_1 = size - head;
    char *p = NULL;
    
    /* head~size-1(第一部分)的长度大于slen */
    if(len_1 >= slen)
    {
        p = StrFind(buffer, s, len_1);
        if(p != NULL)
        {
            FIFO_Offset_Index(fifo, &head, p - buffer);
            if(offset)  fifo->Head_Index = head;
            
            /* 无论Head_Index是否偏移，返回的有效地址都是head指向的位置 */
            return (char *)&fifo->Buffer[head];
        }
        else  /* head~size-1数据中不包含s */
        {
            /* head指向下一步需要判断第一个位置 */
            FIFO_Set_Index(fifo, &head, size - slen + 1);
            
            /* 缓存剩余的有效数据长度 */
            len = len - len_1 + (slen - 1);  

            /* 第一部分剩余的数据长度 */
            len_1 = slen - 1;                          
        }
    }
    
    if(p == NULL)  /* head~size-1的数据不足slen或不包含s */
    {
        /* 通过临时缓存将[size - len_1, size - 1]及[0, len_1]的数据规整 */
        memcpy(tbuffer, (char *)&fifo->Buffer[head], len_1);
        u32 len_2 = MIN((slen - 1) * 2, len) - len_1;
        memcpy((char *)tbuffer + len_1, fifo->Buffer, len_2);
        
        /* tbuffer.Length >= Min((slen - 1) * 2, len) */
        p = StrFind((char *)tbuffer, s, len_1 + len_2);
        if(p != NULL)
        {
            FIFO_Offset_Index(fifo, &head, p - (char *)tbuffer);
        }
        else  /* 临界处不包含s */
        {
            /* 在缓存下半周的数据中继续查找 */
            FIFO_Set_Index(fifo, &head, 0);
            p = StrFind((char *)fifo->Buffer, s, len - len_1);
            if(p != NULL)
            {
                FIFO_Offset_Index(fifo, &head, p - (char *)fifo->Buffer);
            }
            else  /* 缓存数据中不包含s */
            {
                FIFO_Offset_Index(fifo, &head, len - len_1);
                if(offset)  fifo->Head_Index = head;
                return NULL;
            }
        }
    }
    
    if(offset)  fifo->Head_Index = head;
    
    return (char *)&fifo->Buffer[head];
}
/**/

/**
@功能: 读取FIFO中的数据，遇到指定字符串停止
@参数: Type, FIFO对象
       s, 指定字符串
       pdata, 读取数据
       cntout, 超出计数，cntout <= sizeof(pdata), 防止内存溢出
@返回: 读取的数据长度，以字节为单位
@备注: @读取结果中包含指定字符串
       @pdata内存空间≥Min(cntout, Buffer_Length)
*/
u32  FIFO_Read_To(FIFO_Type *fifo, const char *s, void *pdata, u32 cntout)
{
    FIFO_Lock();
    
    u32 slen = strlen(s);
    u32 len = FIFO_Get_DataLength(fifo);
    if(len < slen)
    {
        FIFO_Unlock();
        return 0;
    }
    
    u32 size = FIFO_Get_BufferSize(fifo);
    char *buffer = (char *)&fifo->Buffer[fifo->Head_Index];
    char *p = NULL;
    char *pMin = (char *)fifo->Buffer, *pMax = (char *)&fifo->Buffer[size - 1];
    
    if(FIFO_Data_IsCC(fifo) == True)
    {
        p = FIFO_Find_StringCS(fifo, buffer, s, len, pdata, False);
        if(p == NULL)  
        {
            FIFO_Unlock();
            return 0;
        }
            
        if(p > pMax || p < pMin)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
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
        p = StrFind(buffer, s, len);
        if(p == NULL)  
        {
            FIFO_Unlock();
            return 0;
        }
        
        if(p > pMax || p < pMin)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
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
    
    len = FIFO_Read(fifo, pdata, len);
    
    /* 头指针偏移至指定字符串所在位置的下一个地址，否则会重复读取 */
    FIFO_Offset_Head(fifo, size);
    
    FIFO_Unlock();
    
    return len;
}
/**/

/**
@功能: 读取FIFO中的数据，直到换行符"\r\n"停止
@参数: Type, FIFO对象
       pdata, 读取数据
       cntout, 超出计数，cntout <= sizeof(pdata), 防止内存溢出
@返回: 读取的数据长度，以字节为单位
@备注: @读取结果中包含"\r\n"
       @pdata内存空间≥min(cntout, Buffer_Length)
*/
u32  FIFO_Read_Line(FIFO_Type *fifo, void *pdata, u32 cntout)
{
    return FIFO_Read_To(fifo, "\r\n", pdata, cntout);
}
/**/

/**
@功能: 从FIFO中读取指定字符串之间的数据
@参数: Type, FIFO对象
       s1, 起始字符串
       s2, 结束字符串
       pdata, 读取数据指针
       cntout, 超出计数，cntout <= sizeof(pdata), 防止内存溢出
@返回: 读取的数据长度，以字节为单位
@备注: 1、读取结果中包含首字符串
       2、pdata内存空间≥MAX(2 * (strlen(s1) - 1), 2 * (strlen(s1) - 1), cntout)
*/
u32  FIFO_Read_Inter(FIFO_Type *fifo, const char *s1, const char *s2, void *pdata, u32 cntout)
{
    FIFO_Lock();
    
    u32 slen1 = strlen(s1);
    u32 slen2 = strlen(s2);
    u32 len = FIFO_Get_DataLength(fifo);
    if(len < slen1 + slen2)  
    {
        FIFO_Unlock();
        return 0;                                                               //缓存中没有足够的数据
    }
    
    u32 size = FIFO_Get_BufferSize(fifo);
    char *buffer = (char *)&fifo->Buffer[fifo->Head_Index];
    char *p1 = NULL, *p2 = NULL;
    char *pMin = (char *)fifo->Buffer, *pMax = (char *)&fifo->Buffer[size - 1];
    
    if(FIFO_Data_IsCC(fifo) == True)                                        //缓存数据跨周期
    {
        p1 = FIFO_Find_StringCS(fifo, buffer, s1, len, pdata, True);
        
        /* 缓存中不包含s1，直接退出 */
        if(p1 == NULL)
        {
            FIFO_Unlock();
            return 0;
        }
        
        /* 缓存污染 */
        if(p1 > pMax || p1 < pMin)   
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
            return 0;
        }
        
        if(p1 >= buffer)                                                        //s1还在第一个周期
        {
            len = len - (p1 - buffer) - slen1;                                  //判断s2时的缓存数据长度
            
            if(pMax - p1 > slen1)                                               //第一个数据周期还有足够长的数据
            {
                p2 = FIFO_Find_StringCS(fifo, p1 + slen1, s2, len, pdata, False);
            }
            else
            {
                p2 = StrFind((char *)fifo->Buffer + (slen1 - (pMax - p1)), s2, len);
            }
            
            if(p2 == NULL)
            {
                FIFO_Unlock();
                return 0;
            }
            
            if(p2 > pMax || p2 < pMin)
            {
                FIFO_Clear_Buffer(fifo);
                FIFO_Unlock();
                return 0;
            }
        }
        else                                           //s1已经在第二个周期
        {
            len = len - (p1 + size - buffer) - slen1;
            
            p2 = StrFind(p1 + slen1, s2, len);
            if(p2 == NULL)
            {
                FIFO_Unlock();
                return 0;
            }
            
            if(p2 > pMax)
            {
                FIFO_Clear_Buffer(fifo);
                FIFO_Unlock();
                return 0;
            }
        }
    }
    else
    {
        p1 = StrFind(buffer, s1, len);
        if(p1 == NULL)  
        {
            FIFO_Unlock();
            return 0;
        }
        
        if(p1 > pMax)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
            return 0;
        }
        
        FIFO_Offset_Head(fifo, p1 - buffer);
        
        p2 = StrFind(p1 + slen1, s2, len - slen1);
        if(p2 == NULL)  
        {
            FIFO_Unlock();
            return 0;
        }
        
        if(p2 > pMax)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
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
        FIFO_Clear_Buffer(fifo);
        FIFO_Unlock();
        return 0;
    }
    
    len = FIFO_Read(fifo, pdata, len);
    
    FIFO_Unlock();
    
    return len;
}
/**/

/**
@功能: 通过首字符串与指定的数据长度从FIFO中读取数据
@参数: Type, FIFO对象
       s，首字符串
       num, 读取的数据长度，以字节为单位;该值不包括首字符串长度
       pdata, 读取数据指针
@返回: 读取的数据长度，以字节为单位
@备注: @读取结果中包含首字符串
       @pdata内存空间≥Min(2 * (strlen(s) - 1), Get_Length())
*/
u32  FIFO_Read_Fisrt(FIFO_Type *fifo, const char *s, void *pdata, u32 num)
{
    FIFO_Lock();
    
    u32 slen = strlen(s);
    u32 len = FIFO_Get_DataLength(fifo);
    if(len < slen + num)  
    {
        FIFO_Unlock();
        return 0;                           //缓存中没有足够的数据
    }
    
    u32 size = FIFO_Get_BufferSize(fifo);
    char *buffer = (char *)&fifo->Buffer[fifo->Head_Index];  //缓存起始位置
    char *p = NULL;
    char *pMin = (char *)fifo->Buffer, *pMax = (char *)&fifo->Buffer[size - 1];
    
    if(FIFO_Data_IsCC(fifo) == True)
    {
        p = FIFO_Find_StringCS(fifo, buffer, s, len, pdata, True);
        if(p == NULL)  
        {
            FIFO_Unlock();
            return 0;
        }
        
        if(p > pMax || p < pMin)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
            return 0;
        }
    }
    else
    {
        p = StrFind(buffer, s, len);
        if(p != NULL)
        {
            FIFO_Offset_Head(fifo, p - buffer);    //head指向首字符开始的位置
        }
        else
        {
            FIFO_Offset_Head(fifo, len);           //缓存数据中不存在s
            FIFO_Unlock();
            return 0;
        }
    }
        
    if(FIFO_Get_DataLength(fifo) < slen + num)  
    {
        FIFO_Unlock();
        return 0;
    }
    
    len = FIFO_Read(fifo, pdata, slen + num);      //返回结果中保留s
    
    FIFO_Unlock();
    
    return len;
}
/**/

/**
@功能: 以Head + Length的方式从FIFO中读取数据
@参数: Type, FIFO对象
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
*/
u32 FIFO_Read_HL(FIFO_Type *fifo, const u8 head, u8 length_site, u8 other_num, void *pdata, u32 cntout)
{
    FIFO_Lock();
    
    u32 len = FIFO_Get_DataLength(fifo);
    if(len < length_site + other_num)  
    {
        FIFO_Unlock();
        return 0;                                     //接收数据不足
    }
    
    while(fifo->Buffer[fifo->Head_Index] != head)     //先获取head
    {
        if(--len < length_site + other_num)  
        {
            FIFO_Unlock();
            return 0;                                 //剩余数据不足
        }
        FIFO_Offset_Head(fifo, 1);
    }
    
    u32 head_index = FIFO_Get_Head(fifo);
    u32 packet_len = fifo->Buffer[FIFO_Offset_Index(fifo, &head_index, length_site)];
    if(packet_len + other_num > cntout)               //数据异常
    {
        FIFO_Unlock();
        return 0;                                     //数据接收过快或读取不及时，造成数据堆积，导致异常
    }
    
    if(len < packet_len + other_num)  
    {
        FIFO_Unlock();
        return 0;
    }
    
    len = FIFO_Read(fifo, pdata, packet_len + other_num);
    
    FIFO_Unlock();
    
    return len;
}
/**/

/**
@功能: 在缓存数据中查找指定字符串(不跨数据周期)
@参数: fifo, FIFO对象
       s, 指定字符串
@返回: NULL, 指定的字符串不存在
       其它, 指定字符串在缓存中的位置
@备注: @直接在缓存中判断指定的字符串是否存在
       @需要与FIFO_Clear_Buffer配合才能避免不确定因素(例如数据接收跨周期)
*/
char *FIFO_Find_String(FIFO_Type *fifo, const char *s)
{
    return StrFind((char *)fifo->Buffer, s, fifo->Block_Num * fifo->Block_Size);
}
/**/

/**
@功能: 在缓存数据中查找指定字符串是否存在(跨数据周期)
@参数: fifo, FIFO对象
       s, 指定字符串
       tbuffer, 临时缓冲区，由调用者提供给函数，作为内部数据交换使用
@返回: True, 字符串存在
       False, 字符串不存在
@备注: @tbuffer内存空间≥Min(2 * (strlen(s) - 1), Get_Length())
*/
Bool FIFO_String_IsExist(FIFO_Type *fifo, const char *s, char *tbuffer)
{
    FIFO_Lock();
    
    u32 slen = strlen(s);
    u32 len = FIFO_Get_DataLength(fifo);
    if(len < slen)  
    {
        FIFO_Unlock();
        return False;                                         //缓存中没有足够数据
    }
    
    u32 size = FIFO_Get_BufferSize(fifo);
    char *buffer = (char *)&fifo->Buffer[fifo->Head_Index];   //缓存起始位置
    char *p = NULL;
    char *pMin = (char *)fifo->Buffer, *pMax = (char *)&fifo->Buffer[size - 1];
    
    if(FIFO_Data_IsCC(fifo) == True)
    {
        p = FIFO_Find_StringCS(fifo, buffer, s, len, tbuffer, False);
        if(p == NULL)  
        {
            FIFO_Unlock();
            return False;
        }
        
        if(p > pMax || p < pMin)
        {
            FIFO_Clear_Buffer(fifo);
            FIFO_Unlock();
            return False;
        }
    }
    else
    {
        p = StrFind(buffer, s, len);
        if(p == NULL)  
        {
            FIFO_Unlock();
            return False;
        }
    }
    
    FIFO_Unlock();
    
    return True;
}
/**/

/**
@功能: 清除FIFO缓存
@参数: Type, FIFO对象
@返回: 无
@备注: 仅缓存索引及数据长度归零
*/
void FIFO_Clear_Buffer(FIFO_Type *fifo)
{
    FIFO_Lock();
    fifo->Head_Index = 0;
    fifo->Tail_Index = 0;
    fifo->Full_Flag = False;
    FIFO_Unlock();
}
/**/

/**
@功能: 使用指定数值填充FIFO缓存
@参数: Type, FIFO对象
       value, 指定填充的数值
@返回: 无
@备注: 除缓存索引及数据长度归零外，缓存也会被指定数值填充
*/
void FIFO_Fill_Buffer(FIFO_Type *fifo, u8 value)
{
    FIFO_Lock();
    fifo->Head_Index = 0;
    fifo->Tail_Index = 0;
    fifo->Full_Flag = False;
    memset(fifo->Buffer, value, fifo->Block_Num * fifo->Block_Size);
    FIFO_Unlock();
}
/**/

/**
@功能: 获取Head_Index
@参数: 无
@返回: Head_Index
@备注: 无
*/
__INLINE_STATIC_ u32  FIFO_Get_Head(FIFO_Type *fifo)
{
    return fifo->Head_Index;
}
/**/

/**
@功能: 设置Head_Index
@参数: value, 设置值
@返回: 设置后的Head_Index
@备注: 无
*/
__INLINE_STATIC_ u32  FIFO_Set_Head(FIFO_Type *fifo, u32 value)
{
    fifo->Head_Index = (value < fifo->Block_Num) ? value : (value % fifo->Block_Num);
    return fifo->Head_Index;
}
/**/

/**
@功能: Head_Index偏移
@参数: offset, 偏移值
@返回: 偏移后的Head_Index
@备注: 无
*/
__INLINE_STATIC_ u32  FIFO_Offset_Head(FIFO_Type *fifo, u32 offset)
{
    fifo->Head_Index = (fifo->Head_Index + offset) % fifo->Block_Num;
    return fifo->Head_Index;
}
/**/
