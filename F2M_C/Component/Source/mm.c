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
/**************************************************************************************************
@模块名称：MM
@模块功能：内存管理
@说    明：
@修改记录：
      日期       作者      简述
   2020-09-25     YF       创建版本，支持多块内存池管理、内存管理
   2021-06-01     YF       重新设计了内存池的底层分配逻辑，在不影响效率的前提下，
                           减少了不必要的内存开销
   2022-02-10     YF       增加了对系统内存管理函数的关联，通过初始化进行配置
                                   Copyright (C), 2020,
***************************************************************************************************/
#include "mm.h"
#include "fw_print.h"

#include "libc.h"


#define MM_LOCK_TIMEOUT           100000    //锁超时时间，单位：++操作的一个执行周期
#define MM_SLEEP_TIME             10        //休眠时间，单位：Pool->Sleep的最小延时单元

#define MM_ALIGN_BYTES            4         //内存管理的对齐字节数


#define BCT_NEXT(BCT)             ((MM_BCT_Type *)((u32)(BCT) + (BCT)->Block_Size + sizeof(MM_BCT_Type)))
#define BCT_SIZE(BCT)             ((BCT)->Block_Size + sizeof(MM_BCT_Type))

#define BCT_CLEAR(BCT)\
do{\
    (BCT)->Next = 0;\
    (BCT)->Block_Size = 0;\
    (BCT)->State = 0;\
}while(0)

#define BCT_IDLE(BCT, size)\
do{\
    (BCT)->Next = (BCT);\
    (BCT)->Block_Size = size - sizeof(MM_BCT_Type);\
    (BCT)->State = 0;\
}while(0)

#define BCT_USED(BCT, size)\
do{\
    (BCT)->Next = NULL;\
    (BCT)->Block_Size = size;\
    (BCT)->State = 1;\
}while(0)


static MM_Pool_Type Pool_Def;                                   //默认使用的内存池
static MM_Pool_Type *PP;


static void Pool_Init(MM_Pool_Type *Pool);
static void Pool_Insert(MM_Pool_Type *node);
static void *Pool_Get(void *p);

static u8   List_Get_TableIndex(u32 size);

static void *List_Malloc(MM_Pool_Type *Pool, u32 size);
static void List_Free(void **p);

static Bool List_Node_Insert(MM_List_Type **list, MM_Node_Type *node);
static Bool List_Node_Delete(MM_List_Type **list);

static void List_Release(MM_List_Type **list);

static Bool Pool_Sleep(MM_Pool_Type *Pool);


/**
@功能：内存池初始化
@参数：Pool, 内存池对象
@返回：无
@说明：1、管理表的长度由size决定，保证至少能申请一次size/2的内存块；
       2、在程序运行周期中，该函数只会执行一次
       3、IdleList_Table中存储的是相应内存块的管理链表的指针，初始完成后为NULL
       4、多任务中，应在初始化时，赋予操作对象一个休眠函数及休眠时间
*/
void MM_Pool_Init(MM_Pool_Type *Pool)
{
    u32 size;
    u8 align;
    
    if(Pool->Init_Flag)  return;                                //一个内存池对象仅需要初始化一次
    
    Pool->Lock_Flag = True;                                     //内存池初始化过程中不允许操作内存池
    
    if(Pool->Size == 0)
    {
        LOG_E("内存池大小不能为0\r\n");
        while(1);
    }
    
    if(Pool->Base == NULL)
    {
        void *p = malloc(Pool->Size);
        if(p == NULL)                                           //没有足够内存或内存分配不合理
        {
            LOG_E("内存池未申请到有效内存\r\n");
            while(1);
        }
        Pool->Base = (u32)p;
    }
    
    Pool->IdleList_Table_Length = List_Get_TableIndex(Pool->Size / 2) + 1;
    size = Pool->IdleList_Table_Length * sizeof(MM_List_Type *);//管理链表大小
    if(size < Pool->Size)
    {
        /* 将内存池调整为MM_ALIGN_BYTES对齐，保证IdleList_Table的内存对齐 */
        align = Pool->Base % MM_ALIGN_BYTES;
        if(align)  align = MM_ALIGN_BYTES - align;
        Pool->Base += align;
        Pool->Size -= align;
        
        /* 根据内存池的大小，为空闲表table分配内存(从当前的内存池起始位置开始分配) */
        Pool->IdleList_Table = (MM_List_Type **)Pool->Base;
        
        /* IdleList_Table分配内存后，内存池调整为MM_ALIGN_BYTES对齐 */
        align = size % MM_ALIGN_BYTES;
        if(align)  align = MM_ALIGN_BYTES - align;
        Pool->Base += size + align;
        Pool->Size -= size + align;
    }
    else
    {
        LOG_E("内存分配不足\r\n");
        while(1);
    }
    
    memset(Pool->IdleList_Table, 0, size);
    
    Pool_Init(Pool);
    
    /*  */
    if(Pool->Sleep == NULL)  Pool->Sleep_Time = 0;
    else
    {
        if(Pool->Sleep_Time == 0)  Pool->Sleep_Time = MM_SLEEP_TIME;
    }
    
    Pool->Init_Flag = True;
    Pool->Lock_Flag = False;                                    //初始化完成，开锁
}
/**/

/**
@功能: 从内存池中申请内存
@参数: Pool, 内存池对象
       size, 申请的内存大小，以字节为单位
@返回: NULL, 内存申请失败
       其它, 申请到的内存的起始地址
@说明: 1、该函数申请的内存无法进行管理；
       2、MM_Pool_Malloc()执行后，该函数将失效；
       3、该函数执行成功后，会引起内存池的起始位置和大小的变化
*/
void *MM_Pool_Cut(MM_Pool_Type *Pool, u32 size)
{
    void *p;
    
    if(size == 0 || size > Pool->Size)  
    {
        return NULL;
    }
    
    if(Pool->Unfix_Flag)  return NULL;
    
    while(Pool->Lock_Flag)
    {
        if(Pool_Sleep(Pool))  return NULL;
    }
    Pool->Lock_Flag = True;
    
    /* 保证内存的对齐 */
    size = ALIGN_GET_SIZE(size, 4);
    if(size > Pool->Size)
    {
        Pool->Lock_Flag = False;
        return NULL;
    }
    
    /* MM_Pool_Cut()每次都是从Pool->Base处开始申请 */
    p = (void *)Pool->Base;
    Pool->Base += size;
    Pool->Size -= size;
    
    Pool->Lock_Flag = False;
    
    return p;
}
/**/

/**
@功能: 从内存池中申请内存
@参数: Pool, 内存池对象
       size, 申请的内存大小，单位：Byte
@返回: Null, 内存申请失败
       其它, 申请到的内存的起始地址
@说明: 1、尽量申请2^N(N≥3)大小的内存
       2、内存节点申请->Release->申请的过程中，会出现内存污染。可在每次申请完成后，
          对申请的内存执行擦除
@记录：1、2021-08-18，优化，内存申请失败->内存池释放->内存申请后，不对第二次申请
                            结果进行判断
                       Bug, 多任务中，低优先级任务在申请内存过程中被高优先级任务
                            打断，高优先级的内存锁会一直自锁等待，而低优先级任务
                            无法释放锁，导致内存申请程序无法执行。所以，锁自旋时，
                            应周期性释放CPU,保证低优先级任务可以被执行
*/
void *MM_Pool_Malloc(MM_Pool_Type *Pool, u32 size)
{
    void *p;
    u8 index;
    
    MM_List_Type **list;
    
    if(size == 0)  return NULL;
    
    if(Pool == NULL)  Pool = &Pool_Def;
    
    if(size > Pool->Size)  return NULL;
    
    while(Pool->Lock_Flag)                                      //自旋锁判断
    {
        if(Pool_Sleep(Pool))  return NULL;                      //休眠，出让CPU或者超时退出
    }
    Pool->Lock_Flag = True;                                     //加锁
    
    index = List_Get_TableIndex(size);
    size = 1 << (index + 3);                                    //调整为2^n次数
    if(size > Pool->Size)  
    {
        Pool->Lock_Flag = False; 
        return NULL;
    }
    
    list = Pool->IdleList_Table + index;
    
    if(*list == NULL)
    {
        p = List_Malloc(Pool, size);                            //申请的内存块自身携带有管理块，可以作为链表节点使用
        if(p == NULL)
        {
            MM_Pool_Release(Pool);
            p = List_Malloc(Pool, size);                        //内存池释放后，无论是否申请到内存都返回
        }
    }
    else
    {
        p = (void *)(*list);
        List_Node_Delete(list);                                 //将已申请的节点从管理链表中删除
        p = (void *)((u32)p + sizeof(MM_Node_Type));
    }
    
    Pool->Lock_Flag = False;                                    //开锁
    
    if(p)  Pool->Unfix_Flag = True;                             //只要MM_Pool_Malloc申请成功，MM_Cut即失效
    
    return p;
}
/**/

/**
@功能：释放内存
@参数：p, 需要释放的内存指针
@返回：无
@说明：1、内存块释放后，重新挂载到对应的空闲链表上，而不是释放到内存池
@记录：1、2021-09-01，Bug，当释放一块内存时，若超时，则需持续等待而不是退出，否则
                           用户无法知晓内存是否释放成功，可能导致内存消耗异常
*/
void MM_Pool_Free(void *p)
{
    MM_Node_Type *node;
    MM_Pool_Type *pool;
    u8 index;
    
    if(p == NULL)  return;
    
    pool = (MM_Pool_Type *)Pool_Get(p);
    if(pool == NULL)  return;
    
    /* 链表管理的是内存块的BCT */
    node = (MM_Node_Type *)p - 1;
    if(node == NULL)  return;
    
    index = List_Get_TableIndex(node->Block_Size);
    
    /* 释放内存时，有其它任务占用，则休眠或持续等待 */
    while(pool->Lock_Flag)
    {
        Pool_Sleep(pool);
    }
    pool->Lock_Flag = True;
    List_Node_Insert(pool->IdleList_Table + index, node);
    pool->Lock_Flag = False;
}
/**/

/**
@功能：释放管理表上的内存块
@参数：Pool, 内存池对象
@返回：无
@说明：无
*/
void MM_Pool_Release(MM_Pool_Type *Pool)
{
    u8 i;
    for(i = 0; i < Pool->IdleList_Table_Length; i++)
    {
        List_Release(Pool->IdleList_Table + i);
    }
}
/**/

void MM_Pool_Dispose(MM_Pool_Type *Pool)
{
    
}
/**/

/**
@功能：重映射默认内存池
@参数：Pool, 即将切换至的默认内存池
@返回：True, 切换成功
       False, 切换失败
@说明：1、影响MM_Malloc、MM_Free函数
*/
Bool MM_Remap(MM_Pool_Type *Pool)
{
    if(PP == NULL)
    {
        PP = Pool;
        return True;
    }

    if(PP->Lock_Flag)  return False;                            //等待内存池操作完成后，才能进行切换
    
    PP->Lock_Flag = True;
    PP = Pool;
    PP->Lock_Flag = False;
    
    return True;
}
/**/

__INLINE_STATIC_ void *MM_Def_Cut(u32 size)
{
    return MM_Pool_Cut(&Pool_Def, size);
}

__INLINE_STATIC_ void *MM_Def_Malloc(u32 size)
{
    return MM_Pool_Malloc(&Pool_Def, size);
}

__INLINE_STATIC_ void MM_Def_Free(void *p)
{
    MM_Pool_Free(p);
}

static void *(*pcut)(u32 size) = malloc;
static void *(*pmalloc)(u32 size) = malloc;
static void (*pfree)(void *p) = free;


/**
@功能：默认内存池初始化
@参数：size, 内存池大小，单位：Byte
@返回：无
@说明：1、使用内部RAM作为内存池
       2、size = 0时，MM_Apply、MM_Malloc、MM_Free使用系统函数(不执行该函数效果
          相同)
*/
void MM_Init(u32 size)
{
    if(size == 0)  return;
    
    pcut = MM_Def_Cut;
    pmalloc = MM_Def_Malloc;
    pfree = MM_Def_Free;
    
    PP = &Pool_Def;
    PP->Base = NULL;
    PP->Size = size;
    MM_Pool_Init(PP);
}
/**/

/**
@功能: 从内存池中申请内存
@参数: size, 申请的内存大小，以字节为单位
@返回: NULL, 内存申请失败
       其它, 申请到的内存的起始地址
@说明: 1、PP未初始化的情况下，该函数等价于malloc
*/
void *MM_Cut(u32 size)
{
    return pcut(size);
}
/**/

/**
@功能: 从内存池中申请内存
@参数: size, 申请的内存大小，以字节为单位
@返回: NULL, 内存申请失败
       其它, 申请到的内存的起始地址
@说明: 1、尽量申请8*N大小的内存
       2、内存节点申请->Release->申请的过程中，会出现内存污染。可在每次申请完成
          后，对申请的内存执行擦除
*/
void *MM_Malloc(u32 size)
{
    return pmalloc(size);
}
/**/

/**
@功能：释放内存
@参数：p, 需要释放的内存指针
@返回：无
@说明：1、内存块释放后，重新挂载到对应的空闲链表上，而不是释放到内存池
*/
void MM_Free(void *p)
{
    pfree(p);
    p = NULL;
}
/**/

/**
@功能：获取链表索引
@参数：size, 内存块大小
@返回：无
@说明：1、链表按固定的大小分配，方式如下：
          index 0  1  2  3  4   5   6   7    8    ...
          size  8  16 32 64 128 256 512 1024 2048 ...
*/
static u8 List_Get_TableIndex(u32 size)
{
    u8 index = 0;
    size = (size - 1) >> 3;
    while(size)
    {
        size >>= 1;
        index++;
    }
    return index;
}
/**/

/**
@功能：将新的内存池放入内存池链表中
@参数：Pool, 新的内存池对象
@返回：无
@说明：1、通过链表管理，可以检索到内存所在的内存池
*/
static void Pool_Insert(MM_Pool_Type *Pool)
{
    MM_Pool_Type *list;
    
    if(Pool == &Pool_Def)  return;                              //内存池起始节点
    
    list = &Pool_Def;
    while(list->Next != NULL)  list = list->Next;
    list->Next = Pool;
}
/**/

/**
@功能：检索指针所在的内存池
@参数：p, 给定的指针
@返回：内存池地址
@说明：1、通过链表管理，可以检索到内存所在的内存池
*/
static void *Pool_Get(void *p)
{
    MM_Pool_Type *list = &Pool_Def;
    
    do
    {
        if((u32)p >= list->Base && (u32)p < list->Base + list->Size)
        {
            return (void *)list;
        }
        else
        {
            list = list->Next;
        }
    }while(list);
    
    return NULL;
}
/**/

/**
@功能：内存池初始化
@参数：Pool, 内存池对象
@返回：无
@说明：1、初始化内存池管理的内存空间，并将内存池放入内存池链表中
*/
static void Pool_Init(MM_Pool_Type *Pool)
{
    MM_BCT_Type *BCT = (MM_BCT_Type *)Pool->Base;
    BCT_IDLE(BCT, Pool->Size);
    Pool_Insert(Pool);
}
/**/

/**
@功能：内存池遍历
@参数：Pool, 内存池对象
       size, 申请的内存大小
@返回：NULL, 内存池中没有相应大小的内存块
       其它, 申请到的内存起始地址
@说明：无
*/
static void *Pool_Traverse(MM_Pool_Type *Pool, u32 size)
{
    MM_BCT_Type *BCT, *next;
    
    BCT = (MM_BCT_Type *)Pool->Base;
    
    do
    {
        if(BCT->Next == BCT && BCT->State == 0)                 //内存块空闲
        {
            if(BCT->Block_Size > size)
            {
                next = (MM_BCT_Type *)((u32)BCT + size + sizeof(MM_BCT_Type));
                BCT_IDLE(next, BCT->Block_Size - size);         //切去已分配的内存，剩下的重新组成一个内存块
                BCT_USED(BCT, size);                            //标记已分配的内存
                return (void *)((u32)BCT + sizeof(MM_BCT_Type));
            }
            else if(BCT->Block_Size == size)
            {
                BCT_USED(BCT, size);
                return (void *)((u32)BCT + sizeof(MM_BCT_Type));
            }
            else
            {
                BCT = BCT_NEXT(BCT);                            //当前内存块空间不足，检索下一个
            }
        }
        else                                                    //内存块已占用，检索下一个
        {
            BCT = BCT_NEXT(BCT);
        }
    }while((u32)BCT < Pool->Base + Pool->Size);
    
    return NULL;                                                //已遍历完内存池，无可用内存块
}
/**/

/**
@功能：内存池整理
@参数：Pool, 内存池对象
@返回：无
@说明：1、将空闲的内存块合并
*/
static void Pool_Arrange(MM_Pool_Type *Pool)
{
    MM_BCT_Type *BCT, *next;
    
    BCT = (MM_BCT_Type *)Pool->Base;
    
    do
    {
        if(BCT->Next == BCT && BCT->State == 0)                 //第一块内存块空闲
        {
            next = BCT_NEXT(BCT);
            if(next->Next == next && BCT->State == 0)           //连续两块空闲内存块
            {
                BCT_IDLE(BCT, BCT_SIZE(BCT) + BCT_SIZE(next));  //合并成一个内存块
                BCT_CLEAR(next);
                continue;
            }
            else
            {
                BCT = BCT_NEXT(next);
            }
        }
        else
        {
            BCT = BCT_NEXT(BCT);
        }
    }while((u32)BCT < Pool->Base + Pool->Size);
}
/**/

/**
@功能：从内存池中为动态表中的内存块申请内存
@参数：Pool, 内存池
       size, 申请的内存大小，单位Byte
@返回：Null, 未申请到有效内存
       其它，申请到的内存的起始地址
@说明：1、特殊内存，在无法使用malloc申请时，需要通过Polloc进行内存管理
       2、内存池结构:
           _______ __________ _______ __________ _____ _______ __________
          | BCT_0 | Memory_0 | BCT_1 | Memory_1 | ... | BCT_n | Memory_n | 
          |_______|__________|_______|__________|_____|_______|__________|
*/
static void *Polloc(MM_Pool_Type *Pool, u32 size)
{
    void *p;

#if (POLLOC_TEST == ON)                                         //链表管理时，size已经处理过
    u8 align;
    
    if(size == 0)  return NULL;
    align = size % 8;
    if(align)  align = 8 - align;
    size += align;                                             //申请的内存必须为8的整数倍
#endif
    
    p = Pool_Traverse(Pool, size);
    if(p == NULL)
    {
        Pool_Arrange(Pool);
        p = Pool_Traverse(Pool, size);
        if(p == NULL)  return NULL;
    }
    
    return p;
}
/**/

/**
@功能：释放由Polloc申请的内存
@参数：p, 内存指针
@返回：无
@说明：1、这里的内存直接释放到内存池
       2、内存块标记为空闲即可，无需关心是哪一个内存块申请的
*/
static void Pofree(void *p)
{
    MM_BCT_Type *BCT;
    
    if(p == NULL)  return;
    
    BCT = (MM_BCT_Type *)p - 1;
    BCT_IDLE(BCT, BCT_SIZE(BCT));                               //内存块标记为空闲
}
/**/

/**
@功能：节点内存申请
@参数：Pool, 内存池
       size, 申请的内存大小，单位Byte
@返回：Null, 内存申请失败
       其它, 内存起始地址
@说明：无
*/
static void *List_Malloc(MM_Pool_Type *Pool, u32 size)
{
    return Polloc(Pool, size);
}
/**/

/**
@功能：释放节点内存
@参数：Pool, 内存池
       p, 节点内存指针
@返回：无
@说明：无
*/
static void List_Free(void **p)
{
    Pofree((void **)((u32)(*p) + sizeof(MM_List_Type)));
    *p = NULL;
}
/**/

/**
@功能：向管理表中插入新的节点
@参数：list, 管理表
       node, 待插入的节点
@返回：True, 节点插入成功
       False, 节点插入失败
@说明：1、管理表总是在头部插入新的节点
*/
static Bool List_Node_Insert(MM_List_Type **list, MM_Node_Type *node)
{
    void *p;
    if(node == NULL)  return False;
    p = (void *)(*list);
    *list = node;
    (*list)->Next = (MM_List_Type *)p;
    return True;
}
/**/

/**
@功能：从管理表中删除节点
@参数：list, 管理表
@返回：True, 节点删除成功
       False, 节点删除失败
@说明：1、管理表总是删除头部的节点
*/
static Bool List_Node_Delete(MM_List_Type **list)
{
    if(*list == NULL)  return False;
    *list = (*list)->Next;
    return True;
}
/**/

/**
@功能：释放管理表上的所有节点
@参数：list, 管理表
@返回：无
@说明：1、从头节点开始，轮询释放节点，直到链表为空
*/
static void List_Release(MM_List_Type **list)
{
    void *p = NULL;
    while(*list)
    {
        if(p == (void *)(*list))  break;                        //防止指针异常指向自身，导致无限循环
        p = (void *)((*list)->Next);
        List_Free((void **)list);
        *list = (MM_List_Type *)p;
    }
}
/**/

/**
@功能：任务自锁休眠
@参数：Pool, 内存池
       time, 休眠时间
@返回：True, 超时
       False, 休眠中
@说明：1、当使用超时计数，并有多个任务等待时，计数只会由优先级最高的任务来执行
       2、当计数超时以后，所有等待的任务都将超时退出
*/
static Bool Pool_Sleep(MM_Pool_Type *Pool)
{
    if(Pool->Sleep)
    {
        Pool->Sleep(Pool->Sleep_Time);
    }
    else
    {
        if(++Pool->Sleep_Time >= MM_LOCK_TIMEOUT)  return True;
    }
    return False;
}
/**/


#include "project_config.h"
#if SOFTWARE_TEST && MM_TEST


#define E_VALUE  (*((volatile u32 *)0x200000A8))


#if 1

void Test(void)
{
    u8 *p1, *p2, *p3;
    u8 size = 0;
    u8 index = 0;
    
    MM_Init(1043);
    
    while(1)
    {
        size++;
        p1 = (u8 *)MM_Malloc(size);
        if(p1)  
        {
            memset(p1, 0xDD, size);
        }
        
        size++;
        p2 = (u8 *)MM_Malloc(size);
        if(p2)  
        {
            memset(p2, 0xEE, size);
        }
        
        size++;
        p3 = (u8 *)MM_Malloc(size);
        if(p3)  
        {
            memset(p3, 0xFF, size);
        }
        
        //检测申请内存异常的情况(标记出错)
        if((p1 != NULL && p2 != NULL && p3 != NULL) &&
           (p1 == p2 && p1 == p3 && p2 == p3))
        {
            while(1);
        }
        
        //检测申请不到内存的情况
        if(index >= 200)
        {
            while(1);
        }
        else
        {
            index++;
            
            if(p1 != NULL || p2 != NULL || p3 != NULL)
            {
                index = 0;
            }
        }
        
        MM_Free(p1);
        MM_Free(p2);
        MM_Free(p3);
    }
}

#else

u8 Pool_Buffer[1043];

void Test(void)
{
    u8 *p1, *p2, *p3;
    u16 size = 0;
    
    Pool_Def.Base = (u32)Pool_Buffer;
    Pool_Def.Size = sizeof(Pool_Buffer);
    
    MM_Pool_Init(&Pool_Def);
    
    while(1)
    {
        size++;
        p1 = (u8 *)Polloc(&Pool_Def, size);
        
        size++;
        p2 = (u8 *)Polloc(&Pool_Def, size);
        
        size++;
        p3 = (u8 *)Polloc(&Pool_Def, size);
        
        Pofree(p1);
        Pofree(p2);
        Pofree(p3);
    }
}

void MM_Pool_Lock(MM_Pool_Type *Pool);
void MM_Pool_Unlock(MM_Pool_Type *Pool);
Bool MM_Pool_Lock_Flag(MM_Pool_Type *Pool);

/**
@功能：内存池上锁
@参数：Pool, 内存池对象
@返回：无
@说明：@防止多任务操作内存池冲突
       @内存池上锁时，申请的内存使用完后无论释放与否，都必须解锁
*/
void MM_Pool_Lock(MM_Pool_Type *Pool)
{
    if(Pool == NULL)  Pool = &Pool_Def;
    Pool->Lock_Flag = True;
}
/**/

/**
@功能：内存池开锁
@参数：Pool, 内存池对象
@返回：无
@说明：@防止多任务操作内存池冲突
       @内存池上锁时，申请的内存使用完后无论释放与否，都必须解锁
*/
void MM_Pool_Unlock(MM_Pool_Type *Pool)
{
    if(Pool == NULL)  Pool = &Pool_Def;
    Pool->Lock_Flag = False;
}
/**/

/**
@功能：判断内存池是否上锁
@参数：Pool, 内存池对象
@返回：True, 内存池被锁定
       False, 内存池开放
@说明：@当多任务都会操作内存池时，可以先判断内存池是否可用；不可用时则可以等待或
        执行其它操作
*/
Bool MM_Pool_Lock_Flag(MM_Pool_Type *Pool)
{
    if(Pool == NULL)  Pool = &Pool_Def;
    return (Bool)Pool->Lock_Flag;
}
/**/


#endif


#endif

