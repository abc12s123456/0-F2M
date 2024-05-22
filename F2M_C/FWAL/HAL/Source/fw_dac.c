#include "fw_dac.h"

#include "fw_debug.h"
#include "fw_print.h"
#include "fw_delay.h"

#include "mm.h"



static void FW_DAC_ST_Write(FW_DAC_Type *dev, const void *pdata, u32 num);
static void FW_DAC_HT_Write(FW_DAC_Type *dev, const void *pdata, u32 num);


void FW_DAC_SetPort(FW_DAC_Type *dev, void *dac)
{
    if(dev->DACx == NULL)  dev->DACx = dac;
}

void *FW_DAC_GetPort(FW_DAC_Type *dev)
{
    return dev->DACx;
}

void FW_DAC_DeInit(FW_DAC_Type *dev)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    if(drv->DeInit)  drv->DeInit(dev);
}

void FW_DAC_Init(FW_DAC_Type *dev)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(dev->Resolution <= FW_DAC_Resolution_8Bits)
    {
        dev->Offset = 1;
    }
    else if(dev->Resolution <= FW_DAC_Resolution_16Bits)
    {
        dev->Offset = 2;
    }
    else
    {
        dev->Offset = 4;
    }
    
    if(dev->Buffer_Num)
    {
        if(dev->Buffer == NULL)
        {
            dev->Buffer = MM_Malloc(dev->Buffer_Num * dev->Offset);
            if(dev->Buffer == NULL)
            {
                LOG_D("DAC��������ʧ��\r\n");
                return;
            }
        }
    }
    
    if(dev->Trigger == FW_DAC_Trigger_Software)
    {
        dev->Write = FW_DAC_ST_Write;
    }
    else
    {
        dev->Write = FW_DAC_HT_Write;
    }
    
    if(drv->Trigger_Config)  drv->Trigger_Config(dev, dev->Frequency);
    
    drv->Init(dev);
}

void FW_DAC_CTL(FW_DAC_Type *dev, u8 state)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->CTL(dev, state);
}

void FW_DAC_Write(FW_DAC_Type *dev, const void *pdata, u32 num)
{
//    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
//    u32 i;
//    u8 *p = (u8 *)pdata;
//    
//    for(i = 0; i < num; i++)
//    {
//        drv->Write(dev, *p++);
//        p += dev->Offset;
//    }
    dev->Write(dev, pdata, num);
}

void FW_DAC_TriggerCTL(FW_DAC_Type *dev, u8 state)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Trigger_CTL(dev, state);
}


/* ���������ʽд�� */
static void FW_DAC_ST_Write(FW_DAC_Type *dev, const void *pdata, u32 num)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    u32 i;
    u8 *p = (u8 *)pdata;
    
    for(i = 0; i < num; i++)
    {
        drv->Write(dev, *p++);
        drv->Trigger_CTL(dev, Enable);
        p += dev->Offset;
    }
}

/* Ӳ��������ʽд�� */
static void FW_DAC_HT_Write(FW_DAC_Type *dev, const void *pdata, u32 num)
{
    FW_DAC_Driver_Type *drv = FW_Device_GetDriver(dev);
    u32 i;
    u8 *p = (u8 *)pdata;
    
    if(num > dev->Buffer_Num)  num = dev->Buffer_Num;
    
    /* Buffer����ֱ��ӳ�䵽ֻ���� */
    if(dev->Buffer != pdata)
    {
        memcpy(dev->Buffer, pdata, num * dev->Offset);
    }
    drv->Trigger_CTL(dev, Enable);
}


