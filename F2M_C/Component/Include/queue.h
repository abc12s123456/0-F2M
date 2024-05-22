#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "fw_list.h"


typedef struct
{
    FW_SList_Type *Head;
    FW_SList_Type *Tail;
}Queue_Type;


Queue_Type *Queue_Create(void);
void Queue_Delete(Queue_Type *q);

u32  Queue_Get_Count(Queue_Type *q);

u32  Queue_Write(Queue_Type *q, const void *pdata, u32 num);
u32  Queue_Read(Queue_Type *q, void *pdata);


#ifdef __cplusplus
}
#endif

#endif

