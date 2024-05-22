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

