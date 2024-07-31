#ifndef _SMODEM_SLAVE_H_
#define _SMODEM_SLAVE_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "stiming.h"


typedef enum
{
    Smodem_State_Cancel     = -5,                 //发送方取消
    Smodem_State_ResendErr  = -4,                 //重发超限
    Smodem_State_TimeoutErr = -3,                 //接收超时
    Smodem_State_FileErr    = -2,                 //文件读写出错
    Smodem_State_DataErr    = -1,                 //数据错误
    
    Smodem_State_OK = 0,                          //协议执行完成
    Smodem_State_Start = 1,                       //处理起始帧
    Smodem_State_Data,                            //处理数据帧
    Smodem_State_EOT,                             //
//    Smodem_State_End,                             //处理结束帧
}Smodem_State_Enum;


typedef struct
{
    char *File_Name;                             //文件名称，字符串格式
    u32  File_Size;                              //文件大小
    u32  File_Recv;                              //已接收的数据

    u8   *Packet_Buffer;                         //数据帧缓存
    u32  Packet_Length;                          //数据帧长度，单位：byte
    u32  Packet_Num : 8;                         //数据帧编号
    
    u32  Resend_Count : 8;                       //重发计数
    u32  Err_Count : 8;                          //错误计数
    u32  Timeout_Count : 8;                      //超时计数

    u32  File_Num : 8;                           //传输多个文件时的文件编号
    u32  State : 8;
    u32  Response : 8;                           //应答
    u32  Delay_Mul : 8;                          //延时倍乘数，根据包长确定
    
    STiming_Type Timing;
    
    void (*Packet_TX)(u8 *pdata, u32 num);       //发送数据
    u32  (*Packet_RX)(u8 *pdata, u32 num);       //接收数据
    void (*Packet_Clear)(void);                  //清接收缓存
    
    u32  (*File_Write)(u8 file_num, u32 addr, u8 *pdata, u32 num);
}Smodem_Slave_Type;


void Smodem_Slave_Init(Smodem_Slave_Type *smodem, u32 packet_len);
u8   Smodem_Slave_Handler(Smodem_Slave_Type *smodem);
u8   Smodem_Slave_GetProgress(Smodem_Slave_Type *smodem);
Bool Smodem_Slave_IsFault(Smodem_Slave_Type *smodem);

#ifdef __cplusplus
}
#endif

#endif

