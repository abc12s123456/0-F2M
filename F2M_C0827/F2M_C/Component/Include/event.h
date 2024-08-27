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
#ifndef _EVENT_H_
#define _EVENT_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "fw_list.h"


/* 事件状态 */
typedef enum
{
    Event_State_Suspend = 0,           //挂起
    Event_State_Running,               //运行
}Event_State_Enum;


typedef struct
{
    void (*CB)(void *);
    void *Pdata;
    
    u32  Priotity : 8;
    u32  State : 2;
    
    u32 : 22;
    
    FW_SList_Type List;
}Event_Type;


typedef struct
{
    FW_SList_Type List;
    
    
}Event_Manage_Type;


void Event_Init(Event_Manage_Type *em);                    //
u8   Event_Get_Count(Event_Manage_Type *em);
void Event_Add(Event_Manage_Type *em, Event_Type *e);
void Event_Delete(Event_Manage_Type *em, Event_Type *e);
void Event_Clear(Event_Manage_Type *em);
void Event_Handler(Event_Manage_Type *em);


#ifdef __cplusplus
}
#endif

#endif

