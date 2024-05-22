#ifndef _GYY0709_H_
#define _GYY0709_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    WM_OM_Stop = 0x00,       //����ֹͣ
    WM_OM_LL   = 0x02,       //�������������ȼ�
    WM_OM_LM   = 0x04,       //�������������ȼ�
    WM_OM_LH   = 0x06,       //�������������ȼ�
    WM_OM_HL   = 0x03,       //�������������ȼ�
    WM_OM_HM   = 0x05,       //�������������ȼ�
    WM_OM_HH   = 0x07,       //�������������ȼ�
}WM_OM_Enum;


typedef struct WM WM_Type;

struct WM
{
    FW_Device_Type Device;
    
    u32 L1_Pin : 16;  
    u32 L2_Pin : 16;
    u32 L3_Pin : 16;
    
    u32 : 16;
    
    void (*Init)(WM_Type *dev);
    void (*Output)(WM_Type *dev, WM_OM_Enum om);
};


void WM_Init(WM_Type *dev);
void WM_Output(WM_Type *dev, WM_OM_Enum om);


#ifdef __cplusplus
}
#endif

#endif

