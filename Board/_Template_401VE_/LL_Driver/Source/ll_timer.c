//#include "ll_include.h"
//#include "ll_system.h"
//#include "ll_gpio.h"

//#include "fw_timer.h"
//#include "fw_gpio.h"

//#include "pwm.h"


//#define TIMERx(name)(\
//(Isdev(name, "timer0")) ? TIMER0 :\
//(Isdev(name, "timer1")) ? TIMER1 :\
//(Isdev(name, "timer2")) ? TIMER2 :\
//(Isdev(name, "timer3")) ? TIMER3 :\
//(Isdev(name, "timer4")) ? TIMER4 :\
//(Isdev(name, "timer5")) ? TIMER5 :\
//(Isdev(name, "timer6")) ? TIMER6 :\
//(Isdev(name, "timer7")) ? TIMER7 :\
//((u32)INVALUE))

//#define RCU_TIMERx(timer)(\
//(timer == TIMER0) ? RCU_TIMER0 :\
//(timer == TIMER1) ? RCU_TIMER1 :\
//(timer == TIMER2) ? RCU_TIMER2 :\
//(timer == TIMER3) ? RCU_TIMER3 :\
//(timer == TIMER4) ? RCU_TIMER4 :\
//(timer == TIMER5) ? RCU_TIMER5 :\
//(timer == TIMER6) ? RCU_TIMER6 :\
//(timer == TIMER7) ? RCU_TIMER7 :\
//(rcu_periph_enum)INVALUE)

//#define TIMERx_IRQn(timer)(\
//(timer == TIMER0) ? TIMER0_UP_IRQn :\
//(timer == TIMER1) ? TIMER1_IRQn :\
//(timer == TIMER2) ? TIMER2_IRQn :\
//(timer == TIMER3) ? TIMER3_IRQn :\
//(timer == TIMER4) ? TIMER4_IRQn :\
//(timer == TIMER5) ? TIMER5_IRQn :\
//(timer == TIMER6) ? TIMER6_IRQn :\
//(timer == TIMER7) ? TIMER7_UP_IRQn :\
//(IRQn_Type)INVALUE)

//#define TIMER_CHx(ch)(\
//(ch == FW_TIM_CH0) ? TIMER_CH_0 :\
//(ch == FW_TIM_CH1) ? TIMER_CH_1 :\
//(ch == FW_TIM_CH2) ? TIMER_CH_2 :\
//(ch == FW_TIM_CH3) ? TIMER_CH_3 :\
//INVALUE)

//#define TIMER_INT_CHx(ch)(\
//(ch == FW_TIM_CH0) ? TIMER_INT_CH0 :\
//(ch == FW_TIM_CH1) ? TIMER_INT_CH1 :\
//(ch == FW_TIM_CH2) ? TIMER_INT_CH2 :\
//(ch == FW_TIM_CH3) ? TIMER_INT_CH3 :\
//INVALUE)


//extern FW_TIM_Type TIMER0_Device;
//extern FW_TIM_Type TIMER1_Device;
//extern FW_TIM_Type TIMER2_Device;
//extern FW_TIM_Type TIMER3_Device;
//extern FW_TIM_Type TIMER4_Device;
//extern FW_TIM_Type TIMER5_Device;
//extern FW_TIM_Type TIMER6_Device;
//extern FW_TIM_Type TIMER7_Device;


///* 定时器中断服务函数 */
//__INLINE_STATIC_ u8 TIMER_IRQHandler(FW_TIM_Type *dev)
//{
//    u32 timer = (u32)dev->TIMx;
//    
//    if(timer_interrupt_flag_get(timer, TIMER_INT_FLAG_UP) == SET)
//    {
//        if(dev->IH_CB)  dev->IH_CB(dev->IH_Pdata);
//        timer_interrupt_flag_clear(timer, TIMER_INT_FLAG_UP);
//    }
//    
//    return 0;
//}

//void TIMER0_UP_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER0_Device, TIMER_IRQHandler);
//}

//void TIMER1_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER1_Device, TIMER_IRQHandler);
//}

//void TIMER2_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER2_Device, TIMER_IRQHandler);
//}

//void TIMER3_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER3_Device, TIMER_IRQHandler);
//}

//void TIMER4_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER4_Device, TIMER_IRQHandler);
//}

//void TIMER5_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER5_Device, TIMER_IRQHandler);
//}

//void TIMER6_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER6_Device, TIMER_IRQHandler);
//}

//void TIMER7_UP_IRQHandler(void)
//{
//    FW_DEVICE_LIH(&TIMER7_Device, TIMER_IRQHandler);
//}


///* Timer IO 初始化 */
//__INLINE_STATIC_ void TIM_IO_Init(FW_TIM_Type *dev, u16 pin)
//{
//    u32 timer = (u32)dev->TIMx;
//    
//    if(timer == TIMER0)
//    {
//        if(pin == PA12 || pin == PA8 || pin == PA9 || pin == PA10 || pin == PA11 || 
//           pin == PB12 || pin == PB13 || pin == PB14 || pin == PB15)
//        {}
//        else if(pin == PA6 || pin == PA7 || pin == PB0 || pin == PB1)
//        {
//            gpio_pin_remap_config(GPIO_TIMER0_PARTIAL_REMAP, ENABLE);
//        }
//        else if(pin == PE7 || pin == PE9 || pin == PE11 || pin == PE13 || 
//                pin == PE14 || pin == PE15 || pin == PE8 || pin == PE10 || pin == PE12)
//        {
//            gpio_pin_remap_config(GPIO_TIMER0_FULL_REMAP, ENABLE);
//        }
//        else{}
//    }
//    else if(timer == TIMER1)
//    {
//        if(pin == PA0 || pin == PA1 || pin == PA2 || pin == PA3){}
//        else if(pin == PA15 || pin == PB3)
//        {
//            gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP0, ENABLE);
//        }
//        else if(pin == PB10 || pin == PB11)
//        {
//            gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP1, ENABLE);
//        }
//        else if(pin == PA15 || pin == PB3)
//        {
//            gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);
//        }
//        else{}
//    }
//    else if(timer == TIMER2)
//    {
//        if(pin == PA6 || pin == PA7 || pin == PB0 || pin == PB1){}
//        else if(pin == PB4 || pin == PB5)
//        {
//            gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
//        }
//        else if(pin == PC6 || pin == PC7 || pin == PC8 || pin == PC9)
//        {
//            gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP, ENABLE);
//        }
//        else{}
//    }
//    else if(timer == TIMER3)
//    {
//        if(pin == PB6 || pin == PB7 || pin == PB8 || pin == PB9){}
//        else if(pin == PD12 || pin == PD13 || pin == PD14 || pin == PD15)
//        {
//            gpio_pin_remap_config(GPIO_TIMER3_REMAP, ENABLE);
//        }
//        else{}
//    }
//    else if(timer == TIMER4)
//    {
//        /* PA3作为Timer4 CH3进行IRC40K校准时使用 */
//        if(pin == PA3 && dev->Mode == FW_TIM_Mode_IRCC)
//        {
//            gpio_pin_remap_config(GPIO_TIMER4CH3_IREMAP, ENABLE);
//        }
//    }
//    else
//    {
//        
//    }
//}
///**/

//__INLINE_STATIC_ void TIM_DeInit(FW_TIM_Type *dev)
//{
//    char *name = FW_Device_GetName(dev);
//    u32 timer = (u32)TIMERx(name);
//    timer_deinit(timer);
//}
///**/

//__INLINE_STATIC_ void TIM_Init(FW_TIM_Type *dev)
//{
//    char *name = FW_Device_GetName(dev);
//    u32 timer = (u32)TIMERx(name);
//    FW_TIM_SetPort(dev, (void *)timer);
//}
///**/

//__INLINE_STATIC_ u32  TIM_Get_Clock(FW_TIM_Type *dev)
//{
//    u32 timer = (u32)dev->TIMx;
//    u32 tclk;
//    
//    if(timer == TIMER0 ||
//       timer == TIMER7 ||
//       timer == TIMER8 ||
//       timer == TIMER9 ||
//       timer == TIMER10)
//    {
//        if((RCU_CFG0 & (1 << 13)) != 0)           //RCU_CFG0_APB2PSC 分频系数不为1，则TIMER_CK = 2 * APBx_CK
//        {
//            tclk = LL_Clocks.APB2Clk * 2;
//        }
//        else
//        {
//            tclk = LL_Clocks.APB2Clk;
//        }
//    }
//    else
//    {
//        if((RCU_CFG0 & (1 << 10)) != 0)
//        {
//            tclk = LL_Clocks.APB1Clk * 2;
//        }
//        else
//        {
//            tclk = LL_Clocks.APB1Clk;
//        }
//    }
//    
//    return tclk;
//}
///**/

//__INLINE_STATIC_ void TIM_Set_Frequency(FW_TIM_Type *dev, u32 freq)
//{
//    timer_parameter_struct timer_initpara;
//    u32 timer = (u32)dev->TIMx;
//    u32 tclk = dev->Clock;
//	u32 prescaler = 1, reload = 0;
//    
//    while(1)
//    {
//        reload = tclk / (freq * prescaler);
//        if(reload > 0xFFFF)  prescaler++;
//        else  break;
//    }
//    
//    /* 时基配置 */
//    timer_initpara.prescaler         = prescaler - 1;      //计数器时钟等于定时器时钟除以 (PSC+1)
//    timer_initpara.period            = reload;
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE; //
//    timer_initpara.counterdirection  = TIMER_COUNTER_UP;   //
//    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;   //TIMER_CK与死区时间和采样时钟(DTS)之间的分频系数
//    timer_initpara.repetitioncounter = 0;                  //重复计数器
//    timer_init(timer, &timer_initpara);
//}
///**/

//__INLINE_STATIC_ u32 TIM_Get_Frequency(FW_TIM_Type *dev)
//{
//    u32 timer = (u32)dev->TIMx;
//    u32 tclk = dev->Clock;
//	u32 prescaler, reload;
//    
//    prescaler = timer_prescaler_read(timer);
//    reload = TIMER_CAR((u32)dev->TIMx);
//    
//    return (tclk / (reload * (prescaler + 1)));
//}
///**/

//__INLINE_STATIC_ void TIM_Timing_Init(FW_TIM_Type *dev)
//{
//    char *name = FW_Device_GetName(dev);
//    u32 timer = (u32)TIMERx(name);
//    u32 unit;
//    
//    FW_TIM_SetPort(dev, (void *)timer);
//    
//    timer_disable(timer);
//    
//    /* 使能Timer时钟 */
//    rcu_periph_clock_enable(RCU_TIMERx(timer));
//    
//    /* Timer时钟频率 */
//    FW_TIM_GetClock(dev);
//    
//    /* Timer定时时基单位 */
//    unit = FW_TIM_GetUnit(dev);
//    TIM_Set_Frequency(dev, unit / dev->Timing_Time);
//    
//    /* 中断配置为定时更新，此处只配置，中断开启由IT_CTL操作 */
//    nvic_irq_enable(TIMERx_IRQn(timer), 3, 3);
//    timer_interrupt_enable(timer, TIMER_INT_UP);
//    timer_interrupt_flag_clear(timer, TIMER_INT_FLAG_UP);
//    
//    /* 使能定时器 */
//    timer_auto_reload_shadow_enable(timer);
//    timer_enable(timer);
//}
///**/

//__INLINE_STATIC_ void TIM_PWM_Init(FW_TIM_Type *dev, PWM_Config_Type *config)
//{
//    timer_oc_parameter_struct timer_ocintpara;
//    char *name = FW_Device_GetName(dev);
//    u32 timer = TIMERx(name);
//    u16 channel = TIMER_CHx(config->CH_P);
//    u32 reload;
//    
//    FW_TIM_SetPort(dev, (void *)timer);
//    
//    timer_disable(timer);
//    
//    /* 使能Timer时钟 */
//    rcu_periph_clock_enable(RCU_TIMERx(timer));
//    
//    timer_deinit(timer);
//    
//    /*  */
//    FW_GPIO_Init(config->Pin_P, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//    TIM_IO_Init(dev, config->Pin_P);
//    
//    if(config->OMode == PWM_OMode_Comple)
//    {
//        FW_GPIO_Init(config->Pin_N, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
//        TIM_IO_Init(dev, config->Pin_N);
//    }
//    
//    FW_TIM_GetClock(dev);
//    
//    /* 时基初始化 */
//    TIM_Set_Frequency(dev, config->Frequency);
//    
//    /* 获取PWM的周期计数值 */
//    reload = TIMER_CAR(timer);
//    
//    /* CHx configuration in PWM mode */
//    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;              //输出状态
//    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;        //通道输出极性
//    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;       //空闲状态下输出电平
//    timer_channel_output_config(timer, channel, &timer_ocintpara);
//    
//    if(config->OMode == PWM_OMode_Comple)
//    {
//        timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;         //互补通道输出状态
//        timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;   //互补通道输出极性
//        timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;  //空闲状态下互补通道输出电平
//        timer_channel_output_config(timer, TIMER_CHx(config->CH_P), &timer_ocintpara);
//    }
//    
//    timer_channel_output_pulse_value_config(timer, 
//                                            channel, 
//                                            reload * config->DC / PWM_DC_MAX);
//    timer_channel_output_mode_config(timer, channel, TIMER_OC_MODE_PWM0);       //PWM0:占空P,PWM1:占空N
//    timer_channel_output_shadow_config(timer, channel, TIMER_OC_SHADOW_DISABLE);
//    timer_primary_output_config(timer, ENABLE);
//    timer_channel_output_state_config(timer, channel, TIMER_CCX_DISABLE);       //输出默认关闭，由应用端打开
//    
//    timer_auto_reload_shadow_enable(timer);
//    
//    if(config->IT_EN)
//    {
//        nvic_irq_enable(TIMERx_IRQn(timer), 3, 3);
//        timer_interrupt_enable(timer, TIMER_INT_UP);
//        timer_interrupt_flag_clear(timer, TIMER_INT_FLAG_UP);
//    }
//    
//    timer_enable(timer);
//}
///**/

//__INLINE_STATIC_ void TIM_Encoder_Init(FW_TIM_Type *dev, QEncoder_Config_Type *config)
//{    
//    timer_parameter_struct timer_initpara;
//    timer_ic_parameter_struct timer_icinitpara;
//    u16 ic0_polarity, ic1_polarity;
//    char *name = FW_Device_GetName(dev);
//    u32 timer = TIMERx(name);
//    
//    FW_TIM_SetPort(dev, (void *)timer);
//    
//    timer_disable(timer);
//    
//    /* 使能Timer时钟 */
//    rcu_periph_clock_enable(RCU_TIMERx(timer));
//    
//    /* 时基配置 */
//    timer_initpara.prescaler         = 0;                  //时钟不分频
//    timer_initpara.period            = config->Max;        //编码器计数值
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE; //
//    timer_initpara.counterdirection  = TIMER_COUNTER_UP;   //
//    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;   //TIMER_CK与死区时间和采样时钟(DTS)之间的分频系数
//    timer_initpara.repetitioncounter = 0;                  //重复计数器
//    timer_init(timer, &timer_initpara);
//    
//    /* IO、通道、采样模式配置 */
//    TIM_IO_Init(dev, config->Pin_A);
//    TIM_IO_Init(dev, config->Pin_B);
//    
//    if(config->VL_A == LEVEL_L)
//    {
//        FW_GPIO_Init(config->Pin_A, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
//        ic0_polarity = TIMER_IC_POLARITY_FALLING;
//    }
//    else
//    {
//        FW_GPIO_Init(config->Pin_A, FW_GPIO_Mode_IPD, FW_GPIO_Speed_High);
//        ic0_polarity = TIMER_IC_POLARITY_RISING;
//    }
//    
//    if(config->VL_B == LEVEL_L)
//    {
//        FW_GPIO_Init(config->Pin_B, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
//        ic1_polarity = TIMER_IC_POLARITY_FALLING;
//    }
//    else
//    {
//        FW_GPIO_Init(config->Pin_B, FW_GPIO_Mode_IPD, FW_GPIO_Speed_High);
//        ic1_polarity = TIMER_IC_POLARITY_RISING;
//    }
//    
//    timer_quadrature_decoder_mode_config(timer, TIMER_ENCODER_MODE2, ic0_polarity, ic1_polarity);
//    
//    /*  */
//    timer_channel_input_struct_para_init(&timer_icinitpara);
//    timer_icinitpara.icfilter = 0x0F;
//    timer_input_capture_config(timer, TIMER_CHx(config->CH_A), &timer_icinitpara);
//    timer_input_capture_config(timer, TIMER_CHx(config->CH_B), &timer_icinitpara);
//    
//    /* 起始值 */
//    timer_counter_value_config(timer, 0);
//    
//    timer_enable(timer);
//}
///**/

//__INLINE_STATIC_ void TIM_Set_Counter(FW_TIM_Type *dev, u32 value)
//{
//    if(value > 0xFFFF)  value = 0xFFFF;
//    timer_counter_value_config((u32)dev->TIMx, value);
//}
///**/

//__INLINE_STATIC_ u32  TIM_Get_Counter(FW_TIM_Type *dev)
//{
//    return timer_counter_read((u32)dev->TIMx);
//}
///**/

//__INLINE_STATIC_ void TIM_Set_DC(FW_TIM_Type *dev, u8 ch, u16 dc)
//{
//    u32 timer = (u32)dev->TIMx;
//    u16 reload = TIMER_CAR(timer);
//    u16 channel = TIMER_CHx(ch);
//    timer_channel_output_pulse_value_config(timer, channel, reload * dc / PWM_DC_MAX);
//}
///**/

//__INLINE_STATIC_ u16  TIM_Get_DC(FW_TIM_Type *dev, u8 ch)
//{
//    u32 timer = (u32)dev->TIMx;
//    u16 reload = TIMER_CAR(timer);
//    u16 channel = TIMER_CHx(ch);
//    u32 cv = timer_channel_capture_value_register_read(timer, channel);
//    return (u16)(cv * PWM_DC_MAX / reload);
//}
///**/

//__INLINE_STATIC_ void TIM_CTL(FW_TIM_Type *dev, u8 state)
//{
//    state ?\
//    timer_enable((u32)dev->TIMx) :\
//    timer_disable((u32)dev->TIMx);
//}
///**/

//__INLINE_STATIC_ void TIM_PWM_CTL(FW_TIM_Type *dev, u8 ch, u8 state)
//{
//    u32 timer = (u32)dev->TIMx;
//    u16 channel = TIMER_CHx(ch);
//    state ?\
//    timer_channel_output_state_config(timer, channel, TIMER_CCX_ENABLE) :\
//    timer_channel_output_state_config(timer, channel, TIMER_CCX_DISABLE);
//}
///**/

//__INLINE_STATIC_ void TIM_IT_CTL(FW_TIM_Type *dev, u8 state)
//{
//    u32 timer = (u32)dev->TIMx;
//    state ?\
//    timer_interrupt_enable(timer, TIMERx_IRQn(timer)) :\
//    timer_interrupt_disable(timer, TIMERx_IRQn(timer));
//}
///**/




///* Native Timer Driver */
//__CONST_STATIC_ FW_TIM_Driver_Type TIM_Driver =
//{
//    .DeInit       = TIM_DeInit,
//    
//    .Timing_Init  = TIM_Timing_Init,
//    .PWM_Init     = TIM_PWM_Init,
//    .Encoder_Init = TIM_Encoder_Init,
//    
//    .Get_Clock     = TIM_Get_Clock,
//    
//    .Set_Frequency = TIM_Set_Frequency,
//    .Get_Frequency = TIM_Get_Frequency,
//    
//    .Set_Counter   = TIM_Set_Counter,
//    .Get_Counter   = TIM_Get_Counter,
//    
//    .Set_DC        = TIM_Set_DC,
//    .Get_DC        = TIM_Get_DC,
//    
//    .CTL           = TIM_CTL,
//    .PWM_CTL       = TIM_PWM_CTL,
//    .IT_CTL        = TIM_IT_CTL,
//};
//FW_DRIVER_REGIST("ll->timer", &TIM_Driver, HTIM);


///* Native Timer Device */
//static void LL_Timer_Config(void *dev)
//{
//    FW_Device_SetDriver(dev, &TIM_Driver);
//}

//static FW_TIM_Type TIMER0_Device;
//FW_DEVICE_STATIC_REGIST("timer0", &TIMER0_Device, LL_Timer_Config, TIMER0);

//static FW_TIM_Type TIMER1_Device;
//FW_DEVICE_STATIC_REGIST("timer1", &TIMER1_Device, LL_Timer_Config, TIMER1);

//static FW_TIM_Type TIMER2_Device;
//FW_DEVICE_STATIC_REGIST("timer2", &TIMER2_Device, LL_Timer_Config, TIMER2);

//static FW_TIM_Type TIMER3_Device;
//FW_DEVICE_STATIC_REGIST("timer3", &TIMER3_Device, LL_Timer_Config, TIMER3);

//static FW_TIM_Type TIMER4_Device;
//FW_DEVICE_STATIC_REGIST("timer4", &TIMER4_Device, LL_Timer_Config, TIMER4);

//static FW_TIM_Type TIMER5_Device;
//FW_DEVICE_STATIC_REGIST("timer5", &TIMER5_Device, LL_Timer_Config, TIMER5);

//static FW_TIM_Type TIMER6_Device;
//FW_DEVICE_STATIC_REGIST("timer6", &TIMER6_Device, LL_Timer_Config, TIMER6);

//static FW_TIM_Type TIMER7_Device;
//FW_DEVICE_STATIC_REGIST("timer7", &TIMER7_Device, LL_Timer_Config, TIMER7);

