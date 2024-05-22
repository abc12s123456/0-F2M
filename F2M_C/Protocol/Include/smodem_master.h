#ifndef _SMODEM_MASTER_H_
#define _SMODEM_MASTER_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "stiming.h"


typedef struct
{
    char *File_Name;
    u32  File_Size;
    u32  File_Remain;                            //文件未传输完数据
    
    u8   *Packet_Buffer;                         //数据帧缓存
    u32  Packet_Length;                          //数据帧长度，单位：byte
    u32  Packet_Num : 8;                         //数据帧编号
    
    u32  Resend_Count : 8;                       //重发计数
    u32  Err_Count : 8;                          //错误计数
    u32  Timeout_Count : 8;                      //超时计数
    
    u32  State : 8;
    u32  File_Num : 8;
    u32  : 16;
//    u32  File_Offset : 24;                       //
    void *File_Ptr;
    
    STiming_Type Timing;
    
    void (*Packet_TX)(u8 *pdata, u32 num);       //发送数据
    u32  (*Packet_RX)(u8 *pdata, u32 num);       //接收数据
    void (*Packet_Clear)(void);
    
//    u32  (*File_Read)(u8 file_num, u32 addr, u8 *pdata, u32 num);
    u32  (*File_Read)(void *file, u32 offset, u8 *pdata, u32 num);
}Smodem_Master_Type;


void Smodem_Master_Init(Smodem_Master_Type *smodem, u32 packet_len);
s8   Smodem_Master_Handler(Smodem_Master_Type *smodem);
u8   Smodem_Master_GetProgress(Smodem_Master_Type *smodem);


#ifdef __cplusplus
}
#endif

#endif

