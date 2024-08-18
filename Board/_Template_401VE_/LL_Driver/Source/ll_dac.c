//#include "ll_include.h"

//#include "fw_dac.h"
//#include "fw_gpio.h"




//#define DACx(name)(\
//Isdev(name, "dac0") ? DAC0 :\
//Isdev(name, "dac1") ? DAC1 :\
//(u32)INVALUE)

///* 触发方式，设置为Period时，根据具体硬件设置触发源 */
//#define DAC_TRIGGER_x(trigger)(\
//(trigger == FW_DAC_Trigger_Software) ? DAC_TRIGGER_SOFTWARE :\
//(trigger == FW_DAC_Trigger_Period) ? DAC_TRIGGER_T6_TRGO :\
//(trigger == FW_DAC_Trigger_EXTI) ? DAC_TRIGGER_EXTI_9 :\
//INVALUE)

///* 输出精度，对齐方式 */
//#define DAC_ALIGN_x_x(res, align)(\
//(res == FW_DAC_Resolution_12Bits && align == FW_DAC_Align_Right) ? DAC_ALIGN_12B_R :\
//(res == FW_DAC_Resolution_12Bits && align == FW_DAC_Align_Left) ? DAC_ALIGN_12B_L :\
//(res == FW_DAC_Resolution_8Bits) ? DAC_ALIGN_8B_R :\
//INVALUE)


//__INLINE_STATIC_ void DAC_DeInit(FW_DAC_Type *dev)
//{
//    char *name = FW_Device_GetName(dev);
//    u32 dac = DACx(name);
//    
//    FW_DAC_SetPort(dev, (void *)dac);
//    
//    dac_deinit();
//}

//__INLINE_STATIC_ void DAC_Init(FW_DAC_Type *dev)
//{
//    char *name = FW_Device_GetName(dev);
//    u32 dac = DACx(name);
//    
//    FW_DAC_SetPort(dev, (void *)dac);

//    dac_deinit();
//    
//    rcu_periph_clock_enable(RCU_DAC);
//    
//    FW_GPIO_Init(dev->Pin, FW_GPIO_Mode_AOUT, FW_GPIO_Speed_High);
//    
//    /* 触发方式 */
//    if(dev->Trigger == FW_DAC_Trigger_Software)
//    {
//        dac_trigger_source_config(dac, DAC_TRIGGER_SOFTWARE);
//    }
//    else
//    {
//        dma_parameter_struct dma_struct;
//        dma_channel_enum dma_ch;
//        
//        u32 dac_periph_addr, dac_periph_width; 
//        u32 memory_addr, memory_width, number;
//        
//        rcu_periph_clock_enable(RCU_DMA1);
//        
//        if(dac == DAC0)
//        {
//            dma_ch = DMA_CH2;
//            
//            if(dev->Resolution <= FW_DAC_Resolution_8Bits)
//            {
//                dac_periph_addr = DAC0_R8DH;
//                dac_periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
//                memory_width = DMA_MEMORY_WIDTH_8BIT;
//            }
//            else
//            {
//                if(dev->Align == FW_DAC_Align_Right)
//                {
//                    dac_periph_addr = (u32)&DAC0_R12DH;
//                }
//                else
//                {
//                    dac_periph_addr = (u32)&DAC0_L12DH;
//                }
//                dac_periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//                memory_width = DMA_MEMORY_WIDTH_16BIT;
//            }
//            
//            /* 定时器触发，DAC0和DAC1使用不同的触发源，底层固定配置 */
//            dac_trigger_source_config(dac, DAC_TRIGGER_T5_TRGO);
//        }
//        else
//        {
//            dma_ch = DMA_CH3;
//            
//            if(dev->Resolution <= FW_DAC_Resolution_8Bits)
//            {
//                dac_periph_addr = (u32)&DAC1_R8DH;
//                dac_periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
//                memory_width = DMA_MEMORY_WIDTH_8BIT;
//            }
//            else
//            {
//                if(dev->Align == FW_DAC_Align_Right)
//                {
//                    dac_periph_addr = (u32)&DAC1_R12DH;
//                }
//                else
//                {
//                    dac_periph_addr = (u32)&DAC1_L12DH;
//                }
//                dac_periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//                memory_width = DMA_MEMORY_WIDTH_16BIT;
//            }
//            
//            dac_trigger_source_config(dac, DAC_TRIGGER_T6_TRGO);
//        }
//        
//        memory_addr = (u32)dev->Buffer;
//        number = dev->Buffer_Num;
//        
//        /* clear all the interrupt flags */
//        dma_flag_clear(DMA1, dma_ch, DMA_INTF_GIF);
//        dma_flag_clear(DMA1, dma_ch, DMA_INTF_FTFIF);
//        dma_flag_clear(DMA1, dma_ch, DMA_INTF_HTFIF);
//        dma_flag_clear(DMA1, dma_ch, DMA_INTF_ERRIF);
//        
//        /* configure the DMA1 channel */
//        dma_struct.periph_addr  = dac_periph_addr;
//        dma_struct.periph_width = dac_periph_width;
//        dma_struct.memory_addr  = memory_addr;
//        dma_struct.memory_width = memory_width;
//        dma_struct.number       = number;
//        dma_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
//        dma_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
//        dma_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
//        dma_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
//        dma_init(DMA1, dma_ch, &dma_struct);
//        
////        dma_circulation_enable(DMA1, dma_ch);
//        dma_circulation_disable(DMA1, dma_ch);
//        dma_channel_enable(DMA1, dma_ch);
//        
//        dac_dma_enable(dac);
//    }
//    
//    /* 使能触发 */
//    dac_trigger_enable(dac);
//    
//    /* 添加噪声波 */
//    if(dev->Noise == FW_DAC_Noise_LFSR)
//    {
//        dac_wave_mode_config(dac, DAC_WAVE_MODE_LFSR);
//        dac_lfsr_noise_config(dac, DAC_LFSR_BITS11_0);
//    }
//    else if(dev->Noise == FW_DAC_Noise_Triangle)
//    {
//        dac_wave_mode_config(dac, DAC_WAVE_MODE_TRIANGLE);
//        dac_triangle_noise_config(dac, DAC_TRIANGLE_AMPLITUDE_4095);
//    }
//    else
//    {
//        dac_wave_mode_config(dac, DAC_WAVE_DISABLE);
//    }
//    
//    /* 开启缓冲，降低输出阻抗，增加输出驱动能力 */
//    if(dev->OB_EN)  dac_output_buffer_enable(dac);
//    else  dac_output_buffer_disable(dac);
//    
//    dac_enable(dac);
//    
//    /* 并发转换 */
////    dac_concurrent_enable();
////    dac_concurrent_data_set(dac, 
//}

//__INLINE_STATIC_ void LL_DAC_CTL(FW_DAC_Type *dev, u8 state)
//{
//    u32 dac = (u32)FW_DAC_GetPort(dev);
//    
//    if(state)  dac_enable(dac);
//    else  dac_disable(dac);
//}

//__INLINE_STATIC_ void DAC_Write(FW_DAC_Type *dev, u32 value)
//{
//    u32 dac = (u32)FW_DAC_GetPort(dev);
//    dac_data_set(dac, DAC_ALIGN_x_x(dev->Resolution, dev->Align), (u16)value);
//}

//__INLINE_STATIC_ u32  DAC_Read(FW_DAC_Type *dev)
//{
//    u32 dac = (u32)FW_DAC_GetPort(dev);
//    return dac_output_value_get(dac);
//}

//#include "fw_timer.h"
//__INLINE_STATIC_ void DAC_Trigger_Config(FW_DAC_Type *dev, u32 freq)
//{
//    if(dev->Trigger == FW_DAC_Trigger_Period)
//    {
//        /* 通过timer产生一个周期性触发事件，触发DAC输出 */
//        FW_TIM_Type *pdev;
//        u32 timer;
//        
//        if(dev->TRO_Source == NULL)
//        {
//            u32 dac = (u32)FW_DAC_GetPort(dev);
//            if(dac == DAC0) dev->TRO_Source = FW_Device_Find("timer5");
//            else  dev->TRO_Source = FW_Device_Find("timer6");
//        } 
//        
//        pdev = dev->TRO_Source;
//        
//        pdev->IT_EN = OFF;
//        pdev->EN = OFF;
//        FW_TIM_Init(pdev);
//        
//        timer = (u32)FW_TIM_GetPort(pdev);
//        timer_master_output_trigger_source_select(timer, TIMER_TRI_OUT_SRC_UPDATE);
//        timer_update_event_enable(timer);
//        
//        FW_TIM_SetFrequency(pdev, freq);
//        FW_TIM_CTL(pdev, Enable);
//    }
//    else
//    {
//        /* EXTI触发 */
//    }
//    
////    u32 timer = TIMER5;
////    rcu_periph_clock_enable(RCU_TIMER5);
////    
////    timer_prescaler_config(timer, 0x0F, TIMER_PSC_RELOAD_UPDATE);
////    timer_autoreload_value_config(timer, 0xFF);
////    timer_master_output_trigger_source_select(timer, TIMER_TRI_OUT_SRC_UPDATE);
////    timer_enable(timer);
//}

//__INLINE_STATIC_ void DAC_Trigger_CTL(FW_DAC_Type *dev, u8 state)
//{
//    u32 dac = (u32)FW_DAC_GetPort(dev);
//    dma_channel_enum dma_ch;
//    
//    if(dev->Trigger == FW_DAC_Trigger_Software)
//    {
//        if(state)  dac_software_trigger_enable(dac);
//        else  dac_software_trigger_disable(dac);
//    }
//    else
//    {
//        if(state)
//        {
//            if(dac == DAC0)  dma_ch = DMA_CH2;
//            else  dma_ch = DMA_CH3;
//            
//            if(dma_flag_get(DMA1, dma_ch, DMA_FLAG_FTF) == SET)
//            {
//                dma_flag_clear(DMA1, dma_ch, DMA_FLAG_FTF);
//                dma_channel_disable(DMA1, dma_ch);
//                dma_transfer_number_config(DMA1, dma_ch, dev->Buffer_Num);
//                dma_channel_enable(DMA1, dma_ch);
//            }
//            
//            dac_trigger_enable(dac);
//        }
//        else
//        {
//            dac_trigger_disable(dac);
//        }
//    }
//}



//__CONST_STATIC_ FW_DAC_Driver_Type LL_DAC_Driver =
//{
//    .DeInit = DAC_DeInit,
//    .Init   = DAC_Init,
//    
//    .CTL    = LL_DAC_CTL,
//    
//    .Write  = DAC_Write,
//    .Read   = DAC_Read,
//    
//    .Trigger_Config = DAC_Trigger_Config,
//    .Trigger_CTL = DAC_Trigger_CTL,
//};
//FW_DRIVER_REGIST("ll->dac", &LL_DAC_Driver, HDAC);


//static FW_DAC_Type LL_DAC0;
//static void DAC0_Config(void *dev)
//{
//    FW_DAC_Type *dac = dev;
//    dac->Pin = PA4;
//    FW_Device_SetDriver(dev, &LL_DAC_Driver);
//}
//FW_DEVICE_STATIC_REGIST("dac0", &LL_DAC0, DAC0_Config, DAC0);



//#if 1

//#include "fw_debug.h"
//#if MODULE_TEST && DAC_TEST
//#include "libc.h"
//#include "fw_delay.h"
//#include "fw_uart.h"

//#include "ll_wave.h"


//#define DAC_NAME   "dac0"


//static void DAC_Pre_Init(void *pdata)
//{
//    FW_DAC_Type *dac = FW_Device_Find(DAC_NAME);
//    dac->Align = FW_DAC_Align_Right;
////    dac->Noise = FW_DAC_Noise_LFSR;
//    dac->Noise = FW_DAC_Noise_None;
////    dac->Noise = FW_DAC_Noise_Triangle;
//    dac->Resolution = FW_DAC_Resolution_12Bits;
////    dac->Trigger = FW_DAC_Trigger_Software;
//    dac->Trigger = FW_DAC_Trigger_Period;
//    dac->Frequency = Arrayof(Wave) * 4;
//    dac->OB_EN = ON;
//    
//    dac->Buffer = (u8 *)Wave;
//    dac->Buffer_Num = Arrayof(Wave);
//    /*
//     * 输出波形的频率等于(freq / num)
//     */
//}
//FW_PRE_INIT(DAC_Pre_Init, NULL);



//void Test(void)
//{
//    FW_DAC_Type *dac;
//    u16 sin_buf[512] = {0};
//    u16 i, num;
//    float fv;
//    float inc;
//    
//    u16 VCC_EN = PA2;
//    
//    FW_TIM_Type *timer = FW_Device_Find("timer5");
//    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_SET(VCC_EN);
//    
//    dac = FW_Device_Find(DAC_NAME);
//    if(dac == NULL)  while(1);
//    
//    
//    num = Arrayof(sin_buf);
//    
////    for(i = 0; i < num; i++)
////    {
////        angle = i * (PI / 180.0f);
////        sin_buf[i] = (u16)(sin(angle) * 2000 + 2000);
////    }

////    FW_UART_PrintInit(PA4, 9600);
//    
//    inc = (2 * PI) / num;
//    
//    
//    for(i = 0; i < num; i++)
//    {
//        fv = 2048 * sin(inc * i) + 2048;
//        if(fv > 4095)  fv = 4095;
//        sin_buf[i] = fv;
////        printf("%d\r\n", sin_buf[i]);
//    }
//    
//    
//    FW_DAC_Init(dac);
//    
//    while(1)
//    {
//        FW_DAC_Write(dac, Wave, Arrayof(Wave));
//        FW_Delay_Ms(500);
//        FW_DAC_Write(dac, Wave, Arrayof(Wave));
//        
//        FW_Delay_Ms(2000);
//    }
//}


//#endif


//#else


//#include "fw_debug.h"
//#if MODULE_TEST && DAC_TEST
//#include "libc.h"
//#include "fw_delay.h"
//#include "fw_uart.h"


//#define DAC_NAME   "dac0"

//static void DAC_Pre_Init(void *pdata)
//{
//    FW_DAC_Type *dac = FW_Device_Find(DAC_NAME);
//    dac->Align = FW_DAC_Align_Right;
////    dac->Noise = FW_DAC_Noise_LFSR;
//    dac->Noise = FW_DAC_Noise_None;
//    dac->Noise = FW_DAC_Noise_Triangle;
//    dac->Resolution = FW_DAC_Resolution_12Bits;
////    dac->Trigger = FW_DAC_Trigger_Software;
//    dac->Trigger = FW_DAC_Trigger_Period;
//    dac->Frequency = 2048;
//    dac->OB_EN = ON;
//    
//    dac->Buffer_Num = 512;
//    /*
//     * 输出波形的频率等于(freq / num)
//     */
//}
//FW_PRE_INIT(DAC_Pre_Init, NULL);



//void Test(void)
//{
//    FW_DAC_Type *dac;
//    u16 sin_buf[512] = {0};
//    u16 i, num;
//    float fv;
//    float inc;
//    
//    u16 VCC_EN = PA2;
//    
//    FW_TIM_Type *timer = FW_Device_Find("timer5");
//    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_SET(VCC_EN);
//    
//    dac = FW_Device_Find(DAC_NAME);
//    if(dac == NULL)  while(1);
//    
//    
//    num = Arrayof(sin_buf);
//    
////    for(i = 0; i < num; i++)
////    {
////        angle = i * (PI / 180.0f);
////        sin_buf[i] = (u16)(sin(angle) * 2000 + 2000);
////    }

////    FW_UART_PrintInit(PA4, 9600);
//    
//    inc = (2 * PI) / num;
//    
//    
//    for(i = 0; i < num; i++)
//    {
//        fv = 2048 * sin(inc * i) + 2048;
//        if(fv > 4095)  fv = 4095;
//        sin_buf[i] = fv;
////        printf("%d\r\n", sin_buf[i]);
//    }
//    
//    
//    FW_DAC_Init(dac);
//    
//    while(1)
//    {
//        FW_DAC_Write(dac, sin_buf, num);
////        FW_Delay_Ms(1000);
////        FW_DAC_CTL(dac, Disable);
////        FW_Delay_Ms(1000);
////        FW_DAC_CTL(dac, Enable);
//    }
//}


//#endif

//#endif

