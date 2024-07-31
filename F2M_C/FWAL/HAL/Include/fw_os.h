#ifndef _FW_OS_H_
#define _FW_OS_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


#define OS_DRV_NUM           0

#define FW_OS_Lock           FW_OS_EnterCritical
#define FW_OS_Unlock         FW_OS_ExitCritical


//OS任务状态
typedef enum
{
    FW_OS_TaskState_Runing = 0,                   //运行
    FW_OS_TaskState_Ready,                        //就绪
    FW_OS_TaskState_Blocked,                      //阻塞
    FW_OS_TaskState_Suspended,                    //挂起
    FW_OS_TaskState_Deleted,                      //删除
    FW_OS_TaskState_Invalid,                      //无效态
}FW_OS_TaskState_Enum;


typedef struct
{
    u32 Driver_ID;
    
    __WM_ void (*OS_TickTock)(void);

    __WM_ void (*OS_Init)(void);
    __WM_ void (*OS_Start)(void);
    __WS_ void (*OS_Yield)(void);
    
    __WM_ Bool (*OS_TaskCreate)(void (*entry)(void *pdata), u32 *stk, u16 stk_size, u8 priority);
    __WO_ void (*OS_TaskDelete)(u8 priority);
    __WS_ void (*OS_TaskSuspend)(u8 priority);
    __WS_ void (*OS_TaskResume)(u8 priority);
    
    __WO_ void *(*OS_TaskGetHandle)(u8 priority);                //获取任务句柄（并非对所有OS都适用）
    __WO_ u32  (*OS_TaskGetSHWM)(u8 priority);
    __WO_ FW_OS_TaskState_Enum (*OS_TaskGetState)(u8 priority);
    
    __WO_ u32  (*OS_GetTick)(void);
    __WO_ u32  (*OS_GetTickTime)(void);
    __WM_ void (*OS_DelayTick)(u32 ticks);
    
    __WO_ void (*OS_EnterCritical)(void);
    __WO_ void (*OS_ExitCritical)(void);
    
    __WO_ u8   (*OS_GetTaskNum)(void);
    __WO_ void (*OS_GetTaskList)(char *list);
    
    /* 消息队列 */
    __WO_ void *(*OS_QCreate)(void **start, u16 size);
    __WO_ Bool (*OS_QPost)(void *pevent, void *pmsg);
    __WO_ void *(*OS_QPend)(void *pevent, void* const buffer, u32 timeout, u8 *perr);

    /* 互斥锁 */
    __WS_ void *(*OS_MutexCreate)(void);
    __WS_ void (*OS_MutexDelete)(void *mutex);
    __WS_ Bool (*OS_MutexTake)(void *mutex, u32 timeout);
    __WS_ Bool (*OS_MutexRelease)(void *mutex);
    
    /* 递归锁(Recurs Lock) */
    __WS_ void *(*OS_RLockCreate)(void);
    __WS_ void (*OS_RLockDelete)(void *rlock);
    __WS_ Bool (*OS_RLockTake)(void *rlock, u32 timeout);
    __WS_ Bool (*OS_RLockRelease)(void *rlock);
}FW_OS_Driver_Type;


typedef struct
{
    __WM_ void (*Entry)(void *pdata);             //任务函数
    __WM_ u32  *STK;                              //任务堆栈
    __WM_ u32  STK_Size : 16;                     //任务堆栈大小
    __WM_ u32  Priority : 8;                      //任务优先级
    
    __RO_ u32  Create_Flag : 1;                   //创建标识
    __RE_ u32  : 7;
    
    __RO_ u32  Monitor_Start_Tick;                //监控起始计数
}FW_OS_Task_Type;


void FW_OS_TickTock(void);

void FW_OS_Init(void);
void FW_OS_Start(void);
Bool FW_OS_IsStart(void);
void FW_OS_Yield(void);

Bool FW_OS_TaskCreate(FW_OS_Task_Type *task);
Bool FW_OS_TaskIsCreate(FW_OS_Task_Type *task);
void FW_OS_TaskDelete(FW_OS_Task_Type *task);
void FW_OS_TaskSuspend(FW_OS_Task_Type *task);
void FW_OS_TaskResume(FW_OS_Task_Type *task);
u32  FW_OS_TaskGetSHWM(FW_OS_Task_Type *task);

void FW_OS_TaskSetMonitor(FW_OS_Task_Type *task);
Bool FW_OS_TaskGetMonitor(FW_OS_Task_Type *task, u32 task_period);

u32  FW_OS_GetTick(void);
u32  FW_OS_GetTickTime(void);
void FW_OS_DelayTick(u32 ticks);
void FW_OS_DelayMs(u32 nms);
void FW_OS_Delay(u32 nms);

void FW_OS_EnterCritical(void);
void FW_OS_ExitCritical(void);

void *FW_OS_MutexCreate(void);
void FW_OS_MutexDelete(void *mutex);
Bool FW_OS_MutexTake(void *mutex, u32 timeout);
Bool FW_OS_MutexRelease(void *mutex);

void *FW_OS_RecursLockCreate(void);
void FW_OS_RecursLockDelete(void *rlock);
Bool FW_OS_RecursLockTake(void *rlock, u32 timeout);
Bool FW_OS_RecursLockRelease(void *rlock);


#ifdef __cplusplus
}
#endif

#endif
