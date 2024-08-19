#include "ll_include.h"
#include "ll_system.h"
#include "ll_gpio.h"

#include "fw_i2c.h"
#include "fw_gpio.h"
#include "fw_delay.h"
#include "fw_system.h"


#if defined(I2C_MOD_EN) && I2C_MOD_EN


#define I2Cx(name)(\
(Isdev(name, "hi2c1")) ? I2C1 :\
(I2C_TypeDef *)INVALUE)


__INLINE_STATIC_ void I2C_IO_Init(FW_I2C_Type *dev)
{
    u16 scl = dev->SCL_Pin;
    u16 sda = dev->SDA_Pin;
    
    u8 af_scl;
    u8 af_sda;

    if(scl == PA3 || scl == PA8 || scl == PA10 || scl == PF1)
    {
        af_scl = LL_GPIO_AF_12;
    }
    else if(scl == PB6)
    {
        af_scl = LL_GPIO_AF_6;
    }
    else
    {
        while(1);
    }
    
    if(sda == PA2 || sda == PA7 || sda == PA9 || sda == PF12 || sda == PF0)
    {
        af_sda = LL_GPIO_AF_12;
    }
    else if(sda == PB7)
    {
        af_sda = LL_GPIO_AF_6;
    }
    else
    {
        while(1);
    }
    
    LL_GPIO_PinAFConfig(scl, af_scl);
    LL_GPIO_PinAFConfig(sda, af_sda);
    
    FW_GPIO_Init(scl, FW_GPIO_Mode_AF_Out_ODU, FW_GPIO_Speed_High);
    FW_GPIO_Init(sda, FW_GPIO_Mode_AF_Out_ODU, FW_GPIO_Speed_High);
}

__INLINE_STATIC_ void I2C_Init(FW_I2C_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    I2C_TypeDef *i2c = (I2C_TypeDef *)I2Cx(name);
    
    LL_I2C_InitTypeDef I2C_InitStruct;
    
    FW_I2C_SetPort(dev, i2c);
    
    u8 trm;
    
    /* 启用 I2C1 的外设时钟 */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
    
    /* 复位I2C */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);
    
    I2C_IO_Init(dev);
    
    trm = FW_I2C_GetTRM(dev, TOR_TX);
    if(trm == TRM_INT)
    {
    
    }
    else
    {
    
    }
    
    trm = FW_I2C_GetTRM(dev, TOR_RX);
    if(trm == TRM_INT)
    {
        
    }
    else
    {
    
    }
    
    /* I2C初始化 */
    I2C_InitStruct.ClockSpeed      = dev->Baudrate;
    I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1     = dev->ID;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
    LL_I2C_Init(i2c, &I2C_InitStruct);
}

__INLINE_STATIC_ void I2C_CTL(FW_I2C_Type *dev, u8 state)
{
    I2C_TypeDef *i2c = (I2C_TypeDef *)dev->I2Cx;
    state ? LL_I2C_Enable(i2c) : LL_I2C_Disable(i2c);
}

__INLINE_STATIC_ u32  I2C_Write(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num)
{
    I2C_TypeDef *i2c = (I2C_TypeDef *)dev->I2Cx;
    u32 i;
    
    /* 清pos */
    LL_I2C_DisableBitPOS(i2c);
    
    /* 产生起始位 */
    LL_I2C_GenerateStartCondition(i2c);
    while(LL_I2C_IsActiveFlag_SB(i2c) != SET);
  
    /* 发送从机地址(7bits) */
    LL_I2C_TransmitData8(i2c, (dev->ID & 0xFE));
    while(LL_I2C_IsActiveFlag_ADDR(i2c) != SET);
    LL_I2C_ClearFlag_ADDR(i2c);
    
    /* 发送数据写入地址(1Byte，可以在dev中扩展为更长的地址) */
    LL_I2C_TransmitData8(i2c, addr);
    while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
    
    /* 发送数据 */
    for(i = 0; i < num; i++)
    {
        while(LL_I2C_IsActiveFlag_TXE(i2c) != SET);
        LL_I2C_TransmitData8(i2c, *(pdata + i));
        
        if((LL_I2C_IsActiveFlag_BTF(i2c) == SET) && i < num)
        {
            LL_I2C_TransmitData8(i2c, *(pdata + i));
        }
        
        while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
    }
    
    /* 产生停止位 */
    LL_I2C_GenerateStopCondition(i2c);
    
    return num;
}

__INLINE_STATIC_ u32  I2C_Read(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num)
{
    I2C_TypeDef *i2c = (I2C_TypeDef *)dev->I2Cx;
    u32 tmp;
    
    /* 清pos */
    LL_I2C_DisableBitPOS(i2c);

    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
    
    /* 产生起始位 */
    LL_I2C_GenerateStartCondition(i2c);
    while(LL_I2C_IsActiveFlag_SB(i2c) != SET);
    
    /* 发送从机地址 */
    LL_I2C_TransmitData8(i2c, (dev->ID & 0xFE));
    while(LL_I2C_IsActiveFlag_ADDR(i2c) != 1);
    LL_I2C_ClearFlag_ADDR(i2c);
    
    /* 发送数据读取地址(1Byte，可以在dev中扩展为更长的地址) */
    LL_I2C_TransmitData8(i2c, addr);
    while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
    
    /* 产生起始位 */
    LL_I2C_GenerateStartCondition(i2c);
    while(LL_I2C_IsActiveFlag_SB(i2c) != SET);
    
    /* 发送从机地址 */
    LL_I2C_TransmitData8(i2c, (dev->ID | 0x1));
    while(LL_I2C_IsActiveFlag_ADDR(i2c) != 1);
    LL_I2C_ClearFlag_ADDR(i2c);
    
    /* 接收数据 */
    if(num == 0)
    {
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_GenerateStopCondition(i2c);
    }
    else if(num == 1)
    {
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
        
        FW_Lock();
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_GenerateStopCondition(i2c);
        FW_Unlock();
    }
    else if(num == 2)
    {
        LL_I2C_EnableBitPOS(i2c);
        
        FW_Lock();
        LL_I2C_ClearFlag_ADDR(i2c);
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
        FW_Unlock();
    }
    else
    {
        LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
        LL_I2C_ClearFlag_ADDR(i2c);
    }
    
    tmp = num;
    while(num > 0)
    {
        if(num <= 3)
        {
            if(num == 1)
            {
                while(LL_I2C_IsActiveFlag_RXNE(i2c) != SET);
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
            }
            else if(num == 2)
            {
                while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
                
                FW_Lock();
                LL_I2C_GenerateStopCondition(i2c);
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
                FW_Unlock();
                
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
            }
            else
            {
                while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
                
                LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
                
                FW_Lock();
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
                
                while(LL_I2C_IsActiveFlag_BTF(i2c) != SET);
                LL_I2C_GenerateStopCondition(i2c);
                
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
                FW_Unlock();
                
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
            }
        }
        else
        {
            while(LL_I2C_IsActiveFlag_RXNE(i2c) != SET);
            
            *pdata++ = LL_I2C_ReceiveData8(i2c);
            num--;
            
            if(LL_I2C_IsActiveFlag_BTF(i2c) == SET)
            {
                *pdata++ = LL_I2C_ReceiveData8(i2c);
                num--;
            }
        }
    }
    
    return tmp;
}




const static FW_I2C_Driver_Type I2C_Driver =
{
    .Init = I2C_Init,
    .CTL = I2C_CTL,
    .Write = I2C_Write,
    .Read = I2C_Read,
};
FW_DRIVER_REGIST("ll->i2c", &I2C_Driver, HI2C);




static FW_I2C_Type I2C1_Device;
static void I2C1_Config(void *dev)
{
    FW_Device_SetDriver(&I2C1_Device, (void *)&I2C_Driver);
};
FW_DEVICE_STATIC_REGIST("i2c1", &I2C1_Device, I2C1_Config, I2C1);




#include "project_debug.h"
#if MODULE_TEST && I2C_TEST
#include "fw_exti.h"


void Test(void)
{
    u8 size = 0;
    
    extern FW_EXTI_Driver_Type EXTI_Driver;
    
    size = sizeof(EXTI_Driver);
    if(size > 10)
    {
        size = size;
    }
    
    printf("%d", size);
    
    while(1);
}


#endif

#endif  /* defined(I2C_MOD_EN) && I2C_MOD_EN */

