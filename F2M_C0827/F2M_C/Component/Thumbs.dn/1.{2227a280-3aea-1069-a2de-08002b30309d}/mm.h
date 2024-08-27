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
 
#ifndef _MM_H_
#define _MM_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"


//内存池管理块
typedef struct MM_ListNodeBCT_Struct
{
    __RO_ struct MM_ListNodeBCT_Struct *Next;    //指向下一个管理块的指针
    __RO_ u32 Block_Size : 31;                   //管理的内存大小
    __RO_ u32 State : 1;                         //管理的内存状态，0：未使用；1：已使用
}MM_List_Type, MM_Node_Type, MM_BCT_Type;


//内存池
typedef struct MM_PoolNode_Struct
{
    __WM_ u32 Base;                              //内存池起始地址
    __WM_ u32 Size;                              //内存池大小
    
    __RO_ MM_List_Type **IdleList_Table;         //空闲节点链表的管理Table
    __RO_ u32 IdleList_Table_Length : 8;
    
    __RO_ u32 Init_Flag : 1;
    __RO_ u32 Lock_Flag : 1;
    
    /* 非固定申请标志，False：只申请不释放；True：申请+释放，并且无法再恢复False
       反映到具体操作上就是MM_Cut()在系统初始上电时可以执行，MM_Malloc()执行以后
       MM_Cut()将失效；并且MM_Cut()申请的内存无法进行管理 */
    __RO_ u32 Unfix_Flag : 1;

    __WS_ u32 Sleep_Time : 21;
    __WS_ void (*Sleep)(u32 time);
    
    __RO_ struct MM_PoolNode_Struct *Next;
}MM_Type, MM_Pool_Type;


void MM_Pool_Init(MM_Pool_Type *Pool);
void *MM_Pool_Cut(MM_Pool_Type *Pool, u32 size);
void *MM_Pool_Malloc(MM_Pool_Type *Pool, u32 size);
void MM_Pool_Free(void *p);

void MM_Pool_Release(MM_Pool_Type *Pool);
void MM_Pool_Dispose(MM_Pool_Type *Pool);

Bool MM_Remap(MM_Pool_Type *Pool);

void MM_Init(u32 size);
void *MM_Cut(u32 size);
void *MM_Malloc(u32 size);
void MM_Free(void *p);


#ifdef __cplusplus
}
#endif

#endif

