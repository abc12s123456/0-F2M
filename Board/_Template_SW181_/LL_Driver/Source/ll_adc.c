//#include "ll_include.h"
//#include "ll_system.h"

//#include "fw_adc.h"
//#include "fw_delay.h"



//#define ADCx(name)(\
//(Isdev(name, "adc0")) ? ADC0 :\
//(Isdev(name, "adc1")) ? ADC1 :\
//(Isdev(name, "adc2")) ? ADC2 :\
//(u32)INVALUE)

//#define ADC_RESOLUTION_x(res)(\
//(res == FW_ADC_Resolution_12Bits) ? ADC_RESOLUTION_12B :\
//(res == FW_ADC_Resolution_10Bits) ? ADC_RESOLUTION_10B :\
//(res == FW_ADC_Resolution_8Bits) ? ADC_RESOLUTION_8B :\
//(res == FW_ADC_Resolution_6Bits) ? ADC_RESOLUTION_6B :\
//INVALUE)

//#define ADC_CHx(ch)  (ch)

//#define ADC_SAMPLETIME_x(time)(\
//(time == FW_ADC_Speed_Low) ? ADC_SAMPLETIME_239POINT5 :\
//(time == FW_ADC_Speed_Medium) ? ADC_SAMPLETIME_41POINT5 :\
//(time == FW_ADC_Speed_High) ? ADC_SAMPLETIME_13POINT5 :\
//(time == FW_ADC_Speed_Ultra) ? ADC_SAMPLETIME_1POINT5 :\
//INVALUE)

//#define ADC_IRQn(adc)(\
//(adc == ADC0) ? ADC0_1_IRQn :\
//(adc == ADC1) ? ADC0_1_IRQn :\
//(adc == ADC2) ? ADC2_IRQn :\
//(IRQn_Type)INVALUE)


///* ADC中断服务函数 */
//__INLINE_STATIC_ u8 ADC_IRQHandler(FW_ADC_Bus_Type *bus)
//{
//    /* EOC标志在读取RDATA时，会自动清除，因此在使用DMA模式时，无法通过EOC标志判断 */
//    FW_ADC_Bus_IH(bus);
//    return True;
//}

//static FW_ADC_Bus_Type ADC0_Bus;

//void ADC0_1_IRQHandler(void)
//{
//    ADC_IRQHandler(&ADC0_Bus);
//}


//void ADC2_IRQHandler(void)
//{
//    
//}

//__INLINE_STATIC_ void ADC_Channel_Config(FW_ADC_Bus_Type *bus)
//{
//    FW_ADC_Device_Type *dev = FW_ADC_Bus_GetHead(bus);
//    u32 adc = (u32)bus->ADCx;
//    u8 rank = 0;
//    
//    while(dev)
//    {
//        adc_regular_channel_config(adc, rank, ADC_CHx(dev->Channel), ADC_SAMPLETIME_x(dev->Speed));
//        if(dev->Channel == FW_ADC_CH16 || dev->Channel == FW_ADC_CH17)
//        {
//            adc_tempsensor_vrefint_enable();
//        }
//        rank++;
//        dev = FW_ADC_GetNext(dev);
//    }
//}

//__INLINE_STATIC_ void ADC_Init(FW_ADC_Bus_Type *bus)
//{
//    u32 adc;
//    u32 dma;
//    
//    dma_parameter_struct dma_parameter;
//    dma_channel_enum dma_ch;
//    
//    u8 ch_num = FW_ADC_Bus_GetChannelNum(bus);
//    char *name = FW_Device_GetName(bus);
//    
//    adc = ADCx(name);
//    
//    FW_ADC_Bus_SetPort(bus, (void *)adc);
//    
//    if(adc == ADC0)
//    {
//        rcu_periph_clock_enable(RCU_DMA0);
//        rcu_periph_clock_enable(RCU_ADC0);
//        dma_deinit(DMA0, DMA_CH0);
//        adc_deinit(ADC0);
//        dma = DMA0;
//        dma_ch = DMA_CH0;
//    }
//    else if(adc == ADC1)
//    {
//        /* ADC1不具备DMA传输功能 */
//        adc_deinit(ADC1);
//    }
//    else if(adc == ADC2)
//    {
//        rcu_periph_clock_enable(RCU_DMA1);
//        rcu_periph_clock_enable(RCU_ADC2);
//        dma_deinit(DMA1, DMA_CH4);
//        adc_deinit(ADC2);
//        dma = DMA1;
//        dma_ch = DMA_CH4;
//    }
//    else
//    {
//        while(1);
//    }
//    
//    /* 使用APB2作为ADC的输入时钟, adc_clk <= 40MHz */
//    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
//    bus->Clock = LL_Clocks.APB2Clk / 4;
//    
//    /* DMA配置 */
//    dma_parameter.periph_addr  = (u32)(&ADC_RDATA(adc));
//    dma_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
//    dma_parameter.memory_addr  = (u32)bus->Buffer;
//    dma_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
//    dma_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//    dma_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
//    dma_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
//    dma_parameter.number       = ch_num;
//    dma_parameter.priority     = DMA_PRIORITY_HIGH;
//    dma_init(dma, dma_ch, &dma_parameter);
//    
//    /* DMA循环传输 */
//    dma_circulation_enable(dma, dma_ch);
//    dma_channel_enable(dma, dma_ch);
//    
//    /* ADC配置 */
//    adc_special_function_config(adc, ADC_SCAN_MODE | ADC_CONTINUOUS_MODE, ENABLE);
//    
//    adc_external_trigger_source_config(adc, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
//    adc_data_alignment_config(adc, ADC_DATAALIGN_RIGHT);
//    adc_resolution_config(adc, ADC_RESOLUTION_x(bus->Resolution));
//    adc_mode_config(ADC_MODE_FREE); 
//    adc_channel_length_config(adc, ADC_REGULAR_CHANNEL, ch_num);           
//    
//    adc_dma_mode_enable(adc);
//    
//    /* ADC通道设置 */
//    ADC_Channel_Config(bus);   
//    
//    /* ADC外部触发设置 */
//    adc_external_trigger_config(adc, ADC_REGULAR_CHANNEL, ENABLE);
//    
//    adc_enable(adc);
//    
//    /* 必要的延时，等待ADC初始化完成 */
//    FW_Delay_Ms(1);                              
//    
//    /* GD32F303系列，ADC校准，必须在ADC使能后才可以执行 */
//    adc_calibration_enable(adc);
//    adc_software_trigger_enable(adc, ADC_REGULAR_CHANNEL);
//    
//    /* ADC中断配置(转换完成中断) */
//    nvic_irq_enable(ADC_IRQn(adc), 3, 3);
//    adc_interrupt_enable(adc, ADC_INT_EOC);
//}

//__INLINE_STATIC_ void ADC_CTL(FW_ADC_Bus_Type *bus, u8 state)
//{
//    u32 adc = (u32)bus->ADCx;
//    state ? adc_enable(adc) : adc_disable(adc);
//}

///* ADC运行条件改变(例如， VDDA、 VREFP 以及温度等)，建议重新执行一次校准操作 */
//__INLINE_STATIC_ void ADC_Calibration(FW_ADC_Bus_Type *bus)
//{
//    u32 adc = (u32)bus->ADCx;
//    u8 state;
//    
//    extern void FW_Lock(void);
//    extern void FW_Unlock(void);
//    
//    FW_Lock();
//    
//    state = ADC_CTL1(adc) & 1;    //获取校准前的ADC状态
//    
//    adc_enable(adc);
//    FW_Delay_RoughMs(1);          //临界保护时，正常的ms定时无法执行
//    adc_calibration_enable(adc);
//    
//    ADC_CTL(bus, state);          //恢复校准前的ADC状态
//    
//    FW_Unlock();
//}

///* ADC通道转换时间=采样时间+(采样分辨率+0.5)*ADCCLK周期 */
//__INLINE_STATIC_ u32  ADC_Get_ConvTime(FW_ADC_Bus_Type *bus, FW_ADC_Device_Type *dev)
//{
//    u32 conv_time;
//    u16 sample_period;
//    u16 conv_period;
//    
//    /* 转换周期，单位：ADCCLK周期(参考用户手册12.4.13表10-11) */
//    conv_period = bus->Resolution * 10 + 5;
//    
//    if(dev->Speed == FW_ADC_Speed_Low)
//    {
//        sample_period = 2395;
//    }
//    else if(dev->Speed == FW_ADC_Speed_Medium)
//    {
//        sample_period = 415;
//    }
//    else if(dev->Speed == FW_ADC_Speed_High)
//    {
//        sample_period = 135;
//    }
//    else
//    {
//        sample_period = 15;
//    }
//    
//    /* sample_period和conv_period在前面扩大了10倍，1000000 / adc_clk对应ADC的周期(单位：us) */
//    conv_time = (sample_period + conv_period) * 100 * 1000000 / bus->Clock;
//    
//    /* 返回结果的单位：ns */
//    return conv_time;
//}

//__INLINE_STATIC_ u32  ADC_VrefFormula(FW_ADC_Bus_Type *bus, u32 value)
//{
//    /* ADC电压基准，扩大10000倍 */
//    return (u32)(FW_ADC_Bus_GetFS(bus) * 12000 / value);
//}

////温度计算公式 Temp = (1.43 - Vadc * Vref / 4095) * 1000 / 4.3 + 25
//__INLINE_STATIC_ s32  ADC_TempFormula(FW_ADC_Bus_Type *bus, u32 value)
//{
//    /* 内部温度传感器测量值，跟随Vref扩大10000倍 */
//    return ((s32)(14300 - value * bus->Vref / FW_ADC_Bus_GetFS(bus)) * 1000 / 43 + 250000);
//}

//__INLINE_STATIC_ u32  ADC_Get_Clock(FW_ADC_Bus_Type *bus)
//{
//    /* 根据Init中时钟的配置 */
//    return LL_Clocks.APB2Clk / 4;
//}

//__INLINE_STATIC_ void ADC_WDG_Config(FW_ADC_Bus_Type *bus, FW_ADC_WDG_Type *wdg)
//{
//    
//}




///* ADC驱动实例化 */
//__CONST_STATIC_ FW_ADC_Driver_Type ADC_Driver =
//{
//    .Init         = ADC_Init,
//    .CTL          = ADC_CTL,
//    .Calibration  = ADC_Calibration,
//    .Get_ConvTime = ADC_Get_ConvTime,
//    .Vref_Formula = ADC_VrefFormula,
//    .Temp_Formula = ADC_TempFormula,
//    .Get_Clock    = ADC_Get_Clock,
//    .WDG_Config   = ADC_WDG_Config,
//};
//FW_DRIVER_REGIST("ll->adc", &ADC_Driver, HADC);



///* ADC0包含共18个采样通道(16个外部+2个内部) */
//static u16 ADC0_Buffer[18] = {0};
//static void ADC0_Bus_Config(void *dev)
//{
//    FW_ADC_Bus_Type *bus = dev;
//    bus->Buffer     = ADC0_Buffer;
//    bus->Buffer_Num = Arrayof(ADC0_Buffer);
//    bus->Width      = FW_ADC_Width_16Bits;
//    bus->Resolution = FW_ADC_Resolution_12Bits;
//    bus->Mode       = FW_ADC_Mode_DMA;
//    
//    FW_Device_SetDriver(&ADC0_Bus, &ADC_Driver);
//}
//FW_DEVICE_STATIC_REGIST("adc0", &ADC0_Bus, ADC0_Bus_Config, ADC0);




//#include "project_debug.h"
//#if MODULE_TEST && ADC_TEST
//#include "mm.h"
//#include "fw_gpio.h"


//FW_ADC_Device_Type ADC_Vref;
//void ADC_Vref_Config(void *pdata)
//{
//    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
//    
//    dev->Channel = FW_ADC_CH17;
//    dev->Buffer_Size = 10;
//    dev->Sample_Period = 1000;
//    
//    FW_Device_SetParent(dev, FW_Device_Find("hadc0"));
//}
//FW_DEVICE_STATIC_REGIST("a_vref", &ADC_Vref, ADC_Vref_Config, ADC_Vref);

//FW_ADC_Device_Type ADC_Temp;
//void ADC_Temp_Config(void *pdata)
//{
//    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
//    
//    dev->Channel = FW_ADC_CH16;
//    dev->Buffer_Size = 10;
//    dev->Sample_Period = 1000;
//    
//    FW_Device_SetParent(dev, FW_Device_Find("hadc0"));
//}
//FW_DEVICE_STATIC_REGIST("a_temp", &ADC_Temp, ADC_Temp_Config, ADC_Temp);

//FW_ADC_Device_Type ADC_Vbat;
//void ADC_Vbat_Config(void *pdata)
//{
//    FW_ADC_Device_Type *dev = (FW_ADC_Device_Type *)pdata;
//    
//    dev->Channel = FW_ADC_CH8;
//    dev->Buffer_Size = 10;
//    dev->Sample_Period = 1000;
//    
//    FW_Device_SetParent(dev, FW_Device_Find("hadc0"));
//}
////FW_DEVICE_STATIC_REGIST("a_vbat", &ADC_Vbat, ADC_Vbat_Config, ADC_Vbat);


//u32 value_vref, value_temp, value_vbat;


//void Test(void)
//{
//    FW_ADC_Bus_Type *fd;
//    FW_ADC_Device_Type *fd_vref, *fd_temp, *fd_vbat;
//    u8 num;
//    
////    u16 VCC_EN = PD1;
//    u16 VCC_EN = PC13;
//    
//    MM_Init(4 * 1024);
//    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_SET(VCC_EN);
//    
//    fd_vref = FW_Device_Find("a_vref");
//    FW_ADC_Init(fd_vref);
//    
//    fd_temp = FW_Device_Find("a_temp");
//    FW_ADC_Init(fd_temp);
//    
////    fd_vbat = FW_Device_Find("a_vbat");
////    FW_ADC_Init(fd_vbat);
//    
//    fd = FW_Device_Find("hadc0");
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    FW_ADC_Bus_Init(fd);
//    
//    while(1)
//    {
//        value_vref = FW_ADC_GetVref(fd_vref);
//        value_temp = FW_ADC_GetTemp(fd_temp);
//        
//        if(value_vref || value_temp)
//        {
//            value_vref = value_vref;
//            value_temp = value_temp;
//        }
//        
//        num = FW_ADC_Read(fd_vbat, &value_vbat, 1);
//        if(num)
//        {
//            value_vbat = value_vbat;
//        }
//        
//        FW_Delay_Ms(50);
//    }
//}


//#endif

