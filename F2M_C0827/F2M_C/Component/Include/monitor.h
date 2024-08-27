#ifndef _MONITOR_H_
#define _MONITOR_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"
#include "fw_list.h"

    
typedef struct
{
    __WM_ u32 Period;             //进程执行周期
    __RO_ u32 Start_Tick;         //起始时刻
    __RO_ FW_SList_Type List;     //监控列表
}Monitor_Type;


void Monitor_Init(Monitor_Type *m);
void Monitor_Set(Monitor_Type *m);
Bool Monitor_Get(void);


#ifdef __cplusplus
}
#endif

#endif

