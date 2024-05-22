/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
 
#ifndef _FW_DAC_H_
#define _FW_DAC_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* DAC�ֱ��� */
typedef enum
{
    FW_DAC_Resolution_6Bits = 6,
    FW_DAC_Resolution_7Bits,
    FW_DAC_Resolution_8Bits,
    FW_DAC_Resolution_9Bits,
    FW_DAC_Resolution_10Bits,
    FW_DAC_Resolution_11Bits,
    FW_DAC_Resolution_12Bits,
    FW_DAC_Resolution_13Bits,
    FW_DAC_Resolution_14Bits,
    FW_DAC_Resolution_15Bits,
    FW_DAC_Resolution_16Bits,
}FW_DAC_Resolution_Enum;

/* DAC����ģʽ */
typedef enum
{
    FW_DAC_Trigger_Software = 0,       //�������
    FW_DAC_Trigger_Period,             //���ڴ���
    FW_DAC_Trigger_EXTI,               //�ⲿ����(Timer����ʵ��Ҳ��һ�����ڴ���)
}FW_DAC_Trigger_Enum;

/* DAC���ݶ��뷽ʽ */
typedef enum
{
    FW_DAC_Align_Right,
    FW_DAC_Align_Left,
}FW_DAC_Align_Enum;

/* DAC�������ģʽ */
typedef enum
{
    FW_DAC_Noise_None,                 //�����
    FW_DAC_Noise_LFSR,                 //LFSR
    FW_DAC_Noise_Triangle,             //���ǲ�
}FW_DAC_Noise_Enum;


typedef struct FW_DAC
{
    FW_Device_Type Device;
    
    void *DACx;
    
    void *Buffer;
    u32 Buffer_Num : 16;
    
    u32 Pin : 16;                      //�����IO
    
    u32 Frequency;                     //������ݵ�Ƶ��(����10��ʾÿ�����10����)
    
    u32 Trigger : 2;
    u32 Resolution : 5;                //
    u32 Align : 2;                     //���ݶ��뷽ʽ
    u32 Noise : 2;                     //��ӵ�����
    
    u32 Offset : 3;
    
    u32 OB_EN : 1;                     //�������
    
    u32 : 17;
    
    void *TRO_Source;                  //����Դ
    
    void (*Write)(struct FW_DAC *dev, const void *pdata, u32 num);
}FW_DAC_Type;


typedef struct
{
    void (*DeInit)(FW_DAC_Type *dev);
    void (*Init)(FW_DAC_Type *dev);
    
    void (*CTL)(FW_DAC_Type *dev, u8 state);
    
    void (*Write)(FW_DAC_Type *dev, u32 value);
    u32  (*Read)(FW_DAC_Type *dev);
    
    void (*Trigger_Config)(FW_DAC_Type *dev, u32 freq);
    void (*Trigger_CTL)(FW_DAC_Type *dev, u8 state);
}FW_DAC_Driver_Type;


void FW_DAC_SetPort(FW_DAC_Type *dev, void *dac);
void *FW_DAC_GetPort(FW_DAC_Type *dev);

void FW_DAC_DeInit(FW_DAC_Type *dev);
void FW_DAC_Init(FW_DAC_Type *dev);
void FW_DAC_CTL(FW_DAC_Type *dev, u8 state);

//void FW_DAC_SetOFrequency(FW_DAC_Type *dev, u32 freq);
//void FW_DAC_SetTrigger(FW_DAC_Type *dev, u8 state);

void FW_DAC_Write(FW_DAC_Type *dev, const void *pdata, u32 num);
void FW_DAC_TriggerCTL(FW_DAC_Type *dev, u8 state);


#ifdef __cplusplus
}
#endif

#endif

