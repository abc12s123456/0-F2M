#ifndef _LIB_CTRL_H_
#define _LIB_CTRL_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


/* 
 * 位置PID 
 * u(t) = Kp*e(t) + Ki*∫e(t)dt + Kd*[dt(t)/dt]
 * 符号或表达式            含义
 *     u(t)        PID输出(不是系统输出)
 *      Kp               比例系数
 *      Ki             积分时间系数
 *      Kd             微分时间系数
 *     e(t)        目标值和当前值的差值
 */
typedef struct
{
    /* 比例、积分、微分系数 */
    float Kp;
    float Ki;
    float Kd;
    
    /* 当前、前一次误差 */
    float Ek_0;
    float Ek_1;
    
    /* 积分值 */
    float Integral;
    
    /* 积分极限值 */
    float IMin;
    float IMax;
    
    /* 输出极限值 */
    float OMin;
    float OMax;
}PPID_Type;


void PPID_Init(PPID_Type *pid);
float PPID_Get(PPID_Type *pid, float target, float actual);


/* 增量PID */
typedef struct
{
    /* 比例、积分、微分系数 */
    float Kp;
    float Ki;
    float Kd;
    
    /* 当前、前一次、再前一次误差 */
    float Ek_0;
    float Ek_1;
    float Ek_2;
}IPID_Type;


void IPID_Init(IPID_Type *pid);
float IPID_Get(IPID_Type *pid, float target, float actual);


/* 
 * 步进PID
 * SPt = SPs + k*sl
 * 符号或表达式            含义
 *      SPt            设定值目标值
 *      SPs            设定值起始值
 *       k             步长变化系数
 *       sl                步长
 *
 *     { 1             设定值增加时
 * k = { 0             设定值不变时
 *     { -1            设定值减小时
 *
 * 优点：减少了阶跃信号带来的扰动，避免因突变带来的超调，使被控对象运行稳定
 * 缺点：会让系统响应速度变慢，系统越稳定，响应越慢
 */
typedef struct
{
    /* 设定目标值 */
    float STarget;
    
    float STMax;
    float STMin;
}SPID_Type;


void  SPID_Init(SPID_Type *pid);
float SPID_Get(SPID_Type *pid, float target, float step);


#ifdef __cplusplus
}
#endif

#endif

