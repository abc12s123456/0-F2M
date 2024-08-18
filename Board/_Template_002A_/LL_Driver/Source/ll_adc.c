#include "ll_include.h"
#include "ll_system.h"

#include "fw_adc.h"
#include "fw_delay.h"



#define ADCx(name)(\
(Isdev(name, "hadc1")) ? ADC1 :\
(ADC_TypeDef *)INVALUE)


#define LL_ADC_SAMPLINGTIME(x)(\
(x == FW_ADC_Speed_Low) ? LL_ADC_SAMPLINGTIME_239CYCLES_5 :\
(x == FW_ADC_Speed_Medium) ? LL_ADC_SAMPLINGTIME_41CYCLES_5 :\
(x == FW_ADC_Speed_High) ? LL_ADC_SAMPLINGTIME_13CYCLES_5 :\
(x == FW_ADC_Speed_Ultra) ? LL_ADC_SAMPLINGTIME_3CYCLES_5 :\
INVALUE)


#define LL_ADC_RESOLUTION(x)(\
(x == FW_ADC_Resolution_12Bits) ? LL_ADC_RESOLUTION_12B :\
(x == FW_ADC_Resolution_10Bits) ? LL_ADC_RESOLUTION_10B :\
(x == FW_ADC_Resolution_8Bits) ? LL_ADC_RESOLUTION_8B :\
(x == FW_ADC_Resolution_6Bits) ? LL_ADC_RESOLUTION_6B :\
INVALUE)


//#define ADC_CH(x)(\
//(x == FW_ADC_CH0) ? LL_ADC_CHANNEL_0 :\
//(x == FW_ADC_CH1) ? LL_ADC_CHANNEL_1 :\
//(x == FW_ADC_CH2) ? LL_ADC_CHANNEL_2 :\
//(x == FW_ADC_CH3) ? LL_ADC_CHANNEL_3 :\
//(x == FW_ADC_CH4) ? LL_ADC_CHANNEL_4 :\
//(x == FW_ADC_CH5) ? LL_ADC_CHANNEL_5 :\
//(x == FW_ADC_CH6) ? LL_ADC_CHANNEL_6 :\
//(x == FW_ADC_CH7) ? LL_ADC_CHANNEL_7 :\
//(x == FW_ADC_CH8) ? LL_ADC_CHANNEL_8 :\
//(x == FW_ADC_CH9) ? LL_ADC_CHANNEL_9 :\
//(x == FW_ADC_CH11) ? LL_ADC_CHANNEL_11 :\
//(x == FW_ADC_CH12) ? LL_ADC_CHANNEL_12 :\
//INVALUE)
__INLINE_STATIC_ u32 ADC_CH(u8 ch)
{
    u32 num = 0, bit;
    
    bit = 1 << ch;
    num = ((ch & 0x08) << ADC_CFGR1_AWDCH_Pos) |
          ((ch & 0x04) << ADC_CFGR1_AWDCH_Pos) |
          ((ch & 0x02) << ADC_CFGR1_AWDCH_Pos) |
          ((ch & 0x01) << ADC_CFGR1_AWDCH_Pos);
    
    return (u32)(num | bit);
}


/* ADC中断服务函数 */
__INLINE_STATIC_ u8 ADC_IRQHandler(FW_ADC_Bus_Type *bus)
{
    ADC_TypeDef *adc = (ADC_TypeDef *)bus->ADCx;
    
    /* 单次转换完成 */
    if(LL_ADC_IsActiveFlag_EOC(adc) == SET)
    {
        FW_ADC_Bus_IH(bus);
        LL_ADC_ClearFlag_EOC(adc);
    }
    
    /* 序列转换完成 */
    else if(LL_ADC_IsActiveFlag_EOS(adc) == SET)
    {
        FW_ADC_Bus_SC_IH(bus);
        LL_ADC_ClearFlag_EOS(adc);
        
        /* 设置为单次转换模式，防止过载影响 */
        LL_ADC_Enable(adc);
        LL_ADC_REG_StartConversion(adc);
    }
    
    /* 其它异常 */
    else
    {
        LL_ADC_ClearFlag_AWD(adc);
        LL_ADC_ClearFlag_EOSMP(adc);
        LL_ADC_ClearFlag_OVR(adc);
        return False;
    }
        
    return True;
}

static FW_ADC_Bus_Type ADC1_Bus;

void ADC_COMP_IRQHandler(void)
{
    FW_DEVICE_LIH(&ADC1_Bus, ADC_IRQHandler);
}


__INLINE_STATIC_ void ADC_Channel_Config(FW_ADC_Bus_Type *bus)
{
    FW_ADC_Device_Type *dev = FW_ADC_Bus_GetHead(bus);
    ADC_TypeDef *adc = (ADC_TypeDef *)bus->ADCx;
    u32 reg_value;
    
    while(dev)
    {
        if(dev->Channel == FW_ADC_CH12)
        {
            reg_value = LL_ADC_GetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc));
            LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc), reg_value | LL_ADC_PATH_INTERNAL_VREFINT);
        }
        else if(dev->Channel == FW_ADC_CH11)
        {
            reg_value = LL_ADC_GetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc));
            LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc), reg_value | LL_ADC_PATH_INTERNAL_TEMPSENSOR);
        }
        else
        {
            reg_value = LL_ADC_GetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc));
            LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(adc), reg_value | LL_ADC_PATH_INTERNAL_NONE);
        }
        
        /* 设置通道转换时间 */
        LL_ADC_SetSamplingTimeCommonChannels(adc, LL_ADC_SAMPLINGTIME(dev->Speed));
    
        /* 向转换序列添加采样通道 */
        LL_ADC_REG_SetSequencerChAdd(adc, ADC_CH(dev->Channel));
        
        dev = FW_ADC_GetNext(dev);
    }
}

/* 通过函数指针访问不同分辨下的ADC转换函数(通过底层直接读取也可以) */
//static u32 (*LL_ADC_Read)(ADC_TypeDef *adc);

//__INLINE_STATIC_ u32 LL_ADC_ReadData12(ADC_TypeDef *adc)
//{
//    return LL_ADC_REG_ReadConversionData12(adc);
//}

//__INLINE_STATIC_ u32 LL_ADC_ReadData10(ADC_TypeDef *adc)
//{
//    return LL_ADC_REG_ReadConversionData10(adc);
//}

//__INLINE_STATIC_ u32 LL_ADC_ReadData8(ADC_TypeDef *adc)
//{
//    return LL_ADC_REG_ReadConversionData8(adc);
//}

//__INLINE_STATIC_ u32 LL_ADC_ReadData6(ADC_TypeDef *adc)
//{
//    return LL_ADC_REG_ReadConversionData6(adc);
//}

__INLINE_STATIC_ void ADC_Init(FW_ADC_Bus_Type *bus)
{
    char *name = FW_Device_GetName(bus);
    ADC_TypeDef *adc = ADCx(name);
    
    if(adc != ADC1)  return;
    
    FW_ADC_Bus_SetPort(bus, adc);
    
    /* 使用APB1CLK作为ADC时钟源，APB1CLK = 24M, ADCmax = 12M */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
    LL_ADC_SetClock(adc, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
    bus->Clock = LL_Clocks.APB1Clk / 2;
    
    /* 关闭ADC功能，待初始化完成后再打开 */
    LL_ADC_Disable(adc);
    
    LL_ADC_DeInit(adc);
    
    /* 设置12位分辨率 */
    LL_ADC_SetResolution(adc, LL_ADC_RESOLUTION(bus->Resolution));
//    if(bus->Resolution == FW_ADC_Resolution_12Bits)
//        LL_ADC_Read = LL_ADC_ReadData12;
//    else if(bus->Resolution == FW_ADC_Resolution_10Bits)
//        LL_ADC_Read = LL_ADC_ReadData10;
//    else if(bus->Resolution == FW_ADC_Resolution_8Bits)
//        LL_ADC_Read = LL_ADC_ReadData8;
//    else if(bus->Resolution == FW_ADC_Resolution_6Bits)
//        LL_ADC_Read = LL_ADC_ReadData6;
//    else
//        while(1);
    
    /* 设置数据右对齐 */
    LL_ADC_SetResolution(adc, LL_ADC_DATA_ALIGN_RIGHT);

    /* 设置低功耗模式无 */
    LL_ADC_SetLowPowerMode(adc, LL_ADC_LP_MODE_NONE);
    
    /* 设置触发源 */
    LL_ADC_REG_SetTriggerSource(adc, LL_ADC_REG_TRIG_SOFTWARE);
    
//    /* 设置为持续转换 */
//    LL_ADC_REG_SetContinuousMode(adc, LL_ADC_REG_CONV_CONTINUOUS);
    /* 设置为单次转换，在一个序列完成后，需软件重启一次才能继续转换 */
    LL_ADC_REG_SetContinuousMode(adc, LL_ADC_REG_CONV_SINGLE);
    
    /* 设置扫描方向为从前向后 */
    LL_ADC_REG_SetSequencerScanDirection(adc, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
    
    /* 设置过载管理模式为覆盖上一个值 */
    LL_ADC_REG_SetOverrun(adc, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
//    /* 设置过载管理模式为覆盖上一个值 */
//    LL_ADC_REG_SetOverrun(adc, LL_ADC_REG_OVR_DATA_PRESERVED);

    /* 设置非连续模式为不使能 */
    LL_ADC_REG_SetSequencerDiscont(adc, LL_ADC_REG_SEQ_DISCONT_DISABLE);

    /* 采样通道配置 */
    ADC_Channel_Config(bus);
    
    /* 使能EOC(转换结束)中断 */
    LL_ADC_EnableIT_EOC(adc);
    
    /* 使能EOS(序列转换结束)中断 */
    LL_ADC_EnableIT_EOS(adc);
    
    /* 失能状态下才能进行校准 */
    if(LL_ADC_IsEnabled(adc) == 0)
    {
        LL_ADC_StartCalibration(adc);
        while(LL_ADC_IsCalibrationOnGoing(adc) != 0);
        FW_Delay_Ms(1);
    }

    /* 使能中断通道 */
    NVIC_SetPriority(ADC_COMP_IRQn, 3);
    NVIC_EnableIRQ(ADC_COMP_IRQn);

    /* 使能ADC */
    LL_ADC_Enable(adc);
    FW_Delay_Ms(1);
    
    /* 开启ADC采样转换 */
    LL_ADC_REG_StartConversion(adc);
}

__INLINE_STATIC_ void ADC_CTL(FW_ADC_Bus_Type *bus, u8 state)
{
    ADC_TypeDef *adc = (ADC_TypeDef *)bus->ADCx;
    state ? LL_ADC_Enable(adc) : (void)LL_ADC_Disable(adc);
}

/* ADC运行条件改变(例如， VDDA、 VREFP 以及温度等)，建议重新执行一次校准操作 */
__INLINE_STATIC_ void ADC_Calibration(FW_ADC_Bus_Type *bus)
{
    ADC_TypeDef *adc = (ADC_TypeDef *)bus->ADCx;
    u8 state;
    
    extern void FW_Lock(void);
    extern void FW_Unlock(void);
    
    FW_Lock();
    
    state = LL_ADC_IsEnabled(adc);
    
    LL_ADC_Disable(adc);          //失能状态下才能进行校准
    
    if(LL_ADC_IsEnabled(adc) == 0)
    {
        LL_ADC_StartCalibration(adc);
        while(LL_ADC_IsCalibrationOnGoing(adc) != 0);
        FW_Delay_RoughMs(1);      //临界保护时，正常的ms定时无法执行
    }
    
    ADC_CTL(bus, state);          //恢复校准前的ADC状态
    
    FW_Unlock();
}

/* ADC通道转换时间=采样时间+(采样分辨率+0.5)*ADCCLK周期 */
__INLINE_STATIC_ u32  ADC_Get_ConvTime(FW_ADC_Bus_Type *bus, FW_ADC_Device_Type *dev)
{
    u32 conv_time;
    u16 sample_period;
    u16 conv_period;
    
    conv_period = bus->Resolution * 10 + 5;
    
    if(dev->Speed == FW_ADC_Speed_Low)
    {
        sample_period = 2395;
    }
    else if(dev->Speed == FW_ADC_Speed_Medium)
    {
        sample_period = 415;
    }
    else if(dev->Speed == FW_ADC_Speed_High)
    {
        sample_period = 135;
    }
    else
    {
        sample_period = 35;
    }
    
    /* sample_period和conv_period在前面扩大了10倍，1000000 / adc_clk对应ADC的周期(单位：us) */
    conv_time = (sample_period + conv_period) * 100 * 1000000 / bus->Clock;
    
    /* 返回结果的单位为ns */
    return conv_time;
}

__INLINE_STATIC_ u32  ADC_Read(FW_ADC_Bus_Type *bus)
{
//    return LL_ADC_Read((ADC_TypeDef *)bus->ADCx);
    return (u32)(READ_BIT(((ADC_TypeDef *)bus->ADCx)->DR, ADC_DR_DATA));
}

__INLINE_STATIC_ u32  ADC_VrefFormula(FW_ADC_Bus_Type *bus, u32 value)
{
    /* ADC电压基准，扩大10000倍 */
    return (u32)(FW_ADC_Bus_GetFS(bus) * 12000 / value);
}

//温度计算公式 Temp = [(85 - 30) / (TScal2 - TScal1)] * (TSdata - TScal1) + 30
#define TSCAL1  (*(volatile u32 *)0x1FFF0F14)
#define TSCAL2  (*(volatile u32 *)0x1FFF0F18)
    
__INLINE_STATIC_ s32  ADC_TempFormula(FW_ADC_Bus_Type *bus, u32 value)
{
    /* 内部温度传感器测量值，跟随Vref扩大10000倍 */
    return ((s32)((550000 / (TSCAL2 - TSCAL1)) * (value - TSCAL1) + 300000));
}

__INLINE_STATIC_ u32  ADC_Get_Clock(FW_ADC_Bus_Type *bus)
{
    /* 根据Init中时钟的配置 */
    return LL_Clocks.APB1Clk / 2;
}

__INLINE_STATIC_ void ADC_WDG_Config(FW_ADC_Bus_Type *bus, FW_ADC_WDG_Type *wdg)
{
    
}




const static FW_ADC_Driver_Type ADC_Driver =
{
    .Init         = ADC_Init,
    .CTL          = ADC_CTL,
    .Calibration  = ADC_Calibration,
    .Get_ConvTime = ADC_Get_ConvTime,
    .Read         = ADC_Read,
    .Vref_Formula = ADC_VrefFormula,
    .Temp_Formula = ADC_TempFormula,
    .Get_Clock    = ADC_Get_Clock,
    .WDG_Config   = ADC_WDG_Config,
};
FW_DRIVER_REGIST("ll->adc", &ADC_Driver, ADC);




/* ADC1包含共11个采样通道(9个外部+2个内部) */
static u16 ADC1_Buffer[11] = {0};
static void ADC1_Bus_Config(void *dev)
{
    FW_ADC_Bus_Type *bus = &ADC1_Bus;
    bus->Buffer     = ADC1_Buffer;
    bus->Buffer_Num = Arrayof(ADC1_Buffer);
    bus->Width      = FW_ADC_Width_16Bits;
    bus->Resolution = FW_ADC_Resolution_12Bits;
    bus->Mode       = FW_ADC_Mode_INT;
    
    FW_Device_SetDriver(&ADC1_Bus, &ADC_Driver);
}
FW_DEVICE_STATIC_REGIST("adc1", &ADC1_Bus, ADC1_Bus_Config, ADC1);
//FW_DEVICE_INIT(FW_ADC_Bus_Init, &ADC0_Bus);




#include "project_debug.h"
#if MODULE_TEST && ADC_TEST
#include "mm.h"
#include "fw_gpio.h"


FW_ADC_Device_Type ADC_Vref;
void ADC_Vref_Config(void *pdata)
{
    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
    
    dev->Channel = FW_ADC_CH12;
    dev->FIFO.Block_Num = 10;
    dev->Sample_Period = 1000;
    
    FW_Device_SetParent(dev, FW_Device_Find("hadc1"));
}
FW_DEVICE_STATIC_REGIST("a_vref", &ADC_Vref, ADC_Vref_Config, ADC_Vref);

FW_ADC_Device_Type ADC_Temp;
void ADC_Temp_Config(void *pdata)
{
    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
    
    dev->Channel = FW_ADC_CH11;
    dev->FIFO.Block_Num = 10;
    dev->Sample_Period = 1000;
    
    FW_Device_SetParent(dev, FW_Device_Find("hadc1"));
}
FW_DEVICE_STATIC_REGIST("a_temp", &ADC_Temp, ADC_Temp_Config, ADC_Temp);

FW_ADC_Device_Type ADC_Vbat;
void ADC_Vbat_Config(void *pdata)
{
    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
    
    dev->Channel = FW_ADC_CH8;
    dev->FIFO.Block_Num = 10;
    dev->Sample_Period = 1000;
    
    FW_Device_SetParent(dev, FW_Device_Find("hadc1"));
}
//FW_DEVICE_STATIC_REGIST("a_vbat", &ADC_Vbat, ADC_Vbat_Config, ADC_Vbat);


u32 value_vref, value_temp, value_vbat;


void Test(void)
{
    FW_ADC_Bus_Type *fd;
    FW_ADC_Device_Type *fd_vref, *fd_temp, *fd_vbat;
    u8 num;
    
//    u16 VCC_EN = PD14;

    
//    MM_Init(1 * 1024);
    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_SET(VCC_EN);
    
    fd_vref = FW_Device_Find("a_vref");
    FW_ADC_Init(fd_vref);
    
    fd_temp = FW_Device_Find("a_temp");
    FW_ADC_Init(fd_temp);
    
//    fd_vbat = FW_Device_Find("a_vbat");
//    FW_ADC_Init(fd_vbat);
    
    fd = FW_Device_Find("hadc1");
    if(fd == NULL)
    {
        while(1);
    }
    
    FW_ADC_Bus_Init(fd);
    
    while(1)
    {
        value_vref = FW_ADC_GetVref(fd_vref);
        value_temp = FW_ADC_GetTemp(fd_temp);
        
        if(value_vref || value_temp)
        {
            value_vref = value_vref;
            value_temp = value_temp;
        }
        
        num = FW_ADC_Read(fd_vbat, &value_vbat, 1);
        if(num)
        {
            value_vbat = value_vbat;
        }
        
        FW_Delay_Ms(50);
    }
}


#endif

