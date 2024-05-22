#include "fw_os.h"
#include "fw_print.h"


static FW_OS_Driver_Type *Driver;

volatile static Bool OS_Start_Flag = False;
volatile static u32  OS_Tick_Time = 0;           /* OS心跳节拍的时间，单位：us */


/**
@功能: OS心跳
@参数: 无
@返回: 无
@备注: 该函数由定时器中断函数周期调用，为OS提供心跳
*/
void FW_OS_TickTock(void)
{
    FW_ASSERT(Driver->OS_TickTock);
    
    Driver->OS_TickTock();
}
/**/

/**
@功能: OS初始化
@参数: 无
@返回: 无
@备注: 无
*/
void HAL_OS_Init(void)
{
    Driver = FW_Driver_Find("ll->os");
    if(Driver == NULL)
    {
        /* os driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->OS_Init);
    
    Driver->OS_Init();
    FW_OS_GetTickTime();
}
/**/

/**
@功能: OS开启
@参数: 无
@返回: 无
@备注: 1、OS工作异常，首先考虑任务栈空间分配(一个是任务的空间，另一个是总空间)
*/
void FW_OS_Start(void)
{
    FW_ASSERT(Driver->OS_Start);
    
    OS_Start_Flag = True;
    
    Driver->OS_Start();
    
    extern void FW_Delay_Init(void);
    FW_Delay_Init();
}
/**/

/**
@功能: 判断OS是否已经启动
@参数: 无
@返回: True, 已开启
       False, 未开启
@备注: 无
*/
Bool FW_OS_IsStart(void)
{
    return OS_Start_Flag;
}
/**/

/**
@功能: 上下文切换
@参数: 无
@返回: 无
@备注: 无
*/
void FW_OS_Yield(void)
{
    FW_ASSERT(Driver->OS_Yield);
    
    Driver->OS_Yield();
}
/**/

/**
@功能: OS任务创建
@参数: task, 创建的任务对象
@返回: True, 任务创建成功
       False, 任务创建失败
@备注: 无
*/
Bool FW_OS_TaskCreate(FW_OS_Task_Type *task)
{
    FW_ASSERT(Driver->OS_Task_Create);
    
    if(task->Create_Flag)  return False;
    task->Create_Flag = True;
    
    if(Driver->OS_GetTick)  
    {
        task->Monitor_Start_Tick = Driver->OS_GetTick();
    }
    
    return Driver->OS_TaskCreate(task->Entry, task->STK, task->STK_Size, task->Priority);
}
/**/

/**
@功能: 判断任务是否已创建
@参数: task, 任务对象
@返回: True, 任务已创建
       False, 任务未创建
@备注: 无
*/
Bool FW_OS_TaskIsCreate(FW_OS_Task_Type *task)
{
    return (Bool)task->Create_Flag;
}
/**/

/**
@功能: OS任务删除
@参数: task, 任务对象
@返回: 无
@备注: @通过任务的优先级执行任务删除，不支持相同任务优先级处理
*/
void FW_OS_TaskDelete(FW_OS_Task_Type *task)
{
    FW_ASSERT(Driver->OS_TaskDelete);
    
    task->Create_Flag = False;
    Driver->OS_TaskDelete(task->Priority);
}
/**/

/**
@功能: 任务挂起
@参数: task, 任务对象
@返回: 无
@备注: 被挂起的任务不会得到任何处理器时间，无论该任务具有什么优先级
*/
void FW_OS_TaskSuspend(FW_OS_Task_Type *task)
{
    FW_ASSERT(Driver->OS_TaskSuspend);
    
    Driver->OS_TaskSuspend(task->Priority);
}
/**/

/**
@功能: 恢复挂起任务
@参数: task, 任务对象
@返回: 无
@备注: 通过调用一次或多次HAL_OS_Suspend挂起的任务，调用一次该函数即可恢复运行
*/
void FW_OS_TaskResume(FW_OS_Task_Type *task)
{
    FW_ASSERT(Driver->OS_TaskResume);
    
    Driver->OS_TaskResume(task->Priority);
}
/**/

/**
@功能: 获取任务堆栈的历史剩余最小值
@参数: task, 任务对象
@返回: 无
@备注: 1、该函数用于检查任务从创建好到现在的堆栈使用剩余值，该值越小说明堆栈溢出的可
       能性就越大
       2、该函数一般比较耗时，所以在调试阶段可以使用，产品发布时最好不要使用
*/
u32  FW_OS_TaskGetSHWM(FW_OS_Task_Type *task)
{
    FW_ASSERT(Driver->OS_TaskGetSHWM);
    
    return Driver->OS_TaskGetSHWM(task->Priority);
}
/**/

/**
@功能: 设置任务的监控标识
@参数: task, 任务对象
@返回: 无
@备注: 在当被监控任务的Entry中调用
*/
void FW_OS_TaskSetMonitor(FW_OS_Task_Type *task)
{
    FW_OS_Lock();
    task->Monitor_Start_Tick = Driver->OS_GetTick();
    FW_OS_Unlock();
}
/**/

/**
@功能: 获取被监控任务的监控标识
@参数: task, 被监控的任务对象
       task_period, 被监控任务的运行周期，一般取值稍大一些
@返回: True, 被监控任务运行正常
       False, 被监控任务运行异常
@备注: 一般由高优先级任务监控低优先级任务
*/
Bool FW_OS_TaskGetMonitor(FW_OS_Task_Type *task, u32 task_period)
{
    u32 tick;
    
    tick = Driver->OS_GetTick();
    
    if(tick >= task->Monitor_Start_Tick)
    {
        if((tick - task->Monitor_Start_Tick) >= task_period)
        {
            /* 指定时间内，被监控的任务未运行 */
            return False;
        }
    }
    else
    {
        if((0xFFFFFFFF - task->Monitor_Start_Tick + tick) >= task_period)
        {
            /* 指定时间内，被监控的任务未运行 */
            return False;
        }
    }
    
    return True;
}
/**/

/**
@功能: 获取当前系统心跳节拍
@参数: 无
@返回: 获取当前的心跳值
@备注: @无
*/
u32 FW_OS_GetTick(void)
{
    FW_ASSERT(Driver->OS_GetTick);
    
    return Driver->OS_GetTick();
}
/**/

/**
@功能: 获取当前系统心跳节拍的时间
@参数: 无
@返回: 一个心跳节拍的时间，单位：us
@备注: @无
*/
u32  FW_OS_GetTickTime(void)
{
    FW_ASSERT(Driver->OS_GetTickTime);
    
    if(Driver->OS_GetTickTime)
    {
        OS_Tick_Time = Driver->OS_GetTickTime();
    }
    else
    {
        /* 默认每个节拍1000us */
        OS_Tick_Time = 1000;
    }
    
    return OS_Tick_Time;
}
/**/

/**
@功能: OS任务调度延时
@参数: ticks, 延时节拍数量
@返回: 无
@备注: @一个tick的时间由用户提供的节拍心跳决定
*/
void FW_OS_DelayTick(u32 ticks)
{
    FW_ASSERT(Driver->OS_DelayTick);
    
    if(ticks == 0)  return;
    
    Driver->OS_DelayTick(ticks);
}
/**/

/**
@功能: OS任务调度延时
@参数: nms，延时时间，单位：ms
@返回: 无
@备注: @要求OS与Delay使用相同的时间节拍
*/
void FW_OS_DelayMs(u32 nms)
{
    FW_ASSERT(Driver->OS_Delay_Tick);
    
    if(nms == 0)  return;
    
    /* 调度延时1ms */
    Driver->OS_DelayTick(nms * 1000 / OS_Tick_Time);
}
/**/

void FW_OS_Delay(u32 nms)
{
    FW_ASSERT(Driver->OS_Delay_Tick);
    
    if(nms == 0)  return;
    
    /* 调度延时1ms */
    Driver->OS_DelayTick(nms * 1000 / OS_Tick_Time);
}
/**/

/**
@功能: OS进入临界保护
@参数: 无
@返回: 无
@备注: @需要与HAL_OS_ExitCritical配合使用
*/
void FW_OS_EnterCritical(void)
{
    FW_ASSERT(Driver->OS_EnterCritical);
    
    Driver->OS_EnterCritical();
}
/**/

/**
@功能: OS退出临界保护
@参数: 无
@返回: 无
@备注: @需要与HAL_OS_EnterCritical配合使用
*/
void FW_OS_ExitCritical(void)
{
    FW_ASSERT(Driver->OS_ExitCritical);
    
    Driver->OS_ExitCritical();
}
/**/

/**
@功能: 创建互斥量
@参数: 无
@返回: 创建的互斥量
@备注: 无
*/
void *FW_OS_MutexCreate(void)
{
    FW_ASSERT(Driver->OS_MutexCreate);
    
    return Driver->OS_MutexCreate();
}
/**/

/**
@功能: 删除互斥量
@参数: mutex, 互斥量对象
@返回: 无
@备注: 无
*/
void FW_OS_MutexDelete(void *mutex)
{
    FW_ASSERT(Driver->OS_MutexDelete);
    
    Driver->OS_MutexDelete(mutex);
}
/**/

/**
@功能: 获取互斥量
@参数: mutex, 互斥量对象
       timeout, 超时时间
@返回: 互斥量获取结果, True:获取成功；False:获取成功
@备注: 无
*/
Bool FW_OS_MutexTake(void *mutex, u32 timeout)
{
    FW_ASSERT(Driver->OS_MutexTake);
    
    return Driver->OS_MutexTake(mutex, timeout);
}
/**/

/**
@功能: 释放互斥量
@参数: mutex, 互斥量对象
@返回: 互斥量释放结果，True:获取成功；False:获取成功
@备注: 无
*/
Bool FW_OS_MutexRelease(void *mutex)
{
    FW_ASSERT(Driver->OS_MutexRelease);
    
    return Driver->OS_MutexRelease(mutex);
}
/**/

/**
@功能: 创建递归锁
@参数: 无
@返回: 创建的递归锁
@备注: 无
*/
void *FW_OS_RecursLockCreate(void)
{
    FW_ASSERT(Driver->OS_RLockCreate);
    
    return Driver->OS_RLockCreate();
}
/**/

/**
@功能: 删除递归锁
@参数: mutex, 递归锁对象
@返回: 无
@备注: 无
*/
void FW_OS_RecursLockDelete(void *rlock)
{
    FW_ASSERT(Driver->OS_RLockDelete);
    
    Driver->OS_RLockDelete(rlock);
}
/**/

/**
@功能: 获取递归锁
@参数: rlock, 递归锁对象
       timeout, 超时时间
@返回: 递归锁获取结果, True:获取成功；False:获取成功
@备注: 无
*/
Bool FW_OS_RecursLockTake(void *rlock, u32 timeout)
{
    FW_ASSERT(Driver->OS_RLockTake);
    
    return Driver->OS_RLockTake(rlock, timeout);
}
/**/

/**
@功能: 释放递归锁
@参数: rlock, 递归锁对象
@返回: 递归锁释放结果，True:获取成功；False:获取成功
@备注: 无
*/
Bool FW_OS_RecursLockRelease(void *rlock)
{
    FW_ASSERT(Driver->OS_RLock_Release);
    
    return Driver->OS_RLockRelease(rlock);
}
/**/

