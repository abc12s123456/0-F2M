#ifndef _FW_SUART_H_
#define _FW_SUART_H_

#ifdef __cplusplus
extern "C"{
#endif


/* 模拟要实现的是uart驱动的一个实例 */
#include "fw_device.h"


typedef struct
{
    __RO_ FW_Device_Type Parent;
    
    /* IO */
    u16 TX_Pin;
    u16 RX_Pin;
    
    /* 波特率 */
    u32 Baudrate;
    
    /* 数据帧格式 */
    u32 Data_Bits : 4;
    u32 Stop_Bits : 4;
    u32 Parity :4;
    
    /* 模拟UART的延时时间，单位：us */
    u32 SDTime : 16;
    u32 : 4;
}FW_SUART_Type;


void FW_SUART_Init(FW_SUART_Type *pdev);
u32  FW_SUART_Write(FW_SUART_Type *pdev, const u8 *pdata, u32 num);
u32  FW_SUART_Read(FW_SUART_Type *pdev, u8 *pdata, u32 num);

void FW_SUART_PrintInit(FW_SUART_Type *pdev);
int  FW_SUART_Printf(const char *format, ...);


#ifdef __cplusplus
}
#endif

#endif

