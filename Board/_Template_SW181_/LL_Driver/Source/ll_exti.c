//#include "ll_include.h"
//#include "ll_gpio.h"

//#include "fw_exti.h"
//#include "fw_gpio.h"


//#define EXTIx_LINE(pin)  ((exti_line_enum)GPIO_PINx(pin))

//#define EXTIx_IRQn(pin)(\
//(GPIO_PINx(pin) == GPIO_PIN_0) ? EXTI0_IRQn :\
//(GPIO_PINx(pin) == GPIO_PIN_1) ? EXTI1_IRQn :\
//(GPIO_PINx(pin) == GPIO_PIN_2) ? EXTI2_IRQn :\
//(GPIO_PINx(pin) == GPIO_PIN_3) ? EXTI3_IRQn :\
//(GPIO_PINx(pin) == GPIO_PIN_4) ? EXTI4_IRQn :\
//(GPIO_PINx(pin) >= GPIO_PIN_5 && GPIO_PINx(pin) <= GPIO_PIN_9) ? EXTI5_9_IRQn :\
//(GPIO_PINx(pin) >= GPIO_PIN_10 && GPIO_PINx(pin) <= GPIO_PIN_15) ? EXTI10_15_IRQn :\
//(IRQn_Type)INVALUE)


//static FW_EXTI_IRQ_Type IRQ_EXTI0;
//static FW_EXTI_IRQ_Type IRQ_EXTI1;
//static FW_EXTI_IRQ_Type IRQ_EXTI2;
//static FW_EXTI_IRQ_Type IRQ_EXTI3;
//static FW_EXTI_IRQ_Type IRQ_EXTI4;
//static FW_EXTI_IRQ_Type IRQ_EXTI5;
//static FW_EXTI_IRQ_Type IRQ_EXTI6;
//static FW_EXTI_IRQ_Type IRQ_EXTI7;
//static FW_EXTI_IRQ_Type IRQ_EXTI8;
//static FW_EXTI_IRQ_Type IRQ_EXTI9;
//static FW_EXTI_IRQ_Type IRQ_EXTI10;
//static FW_EXTI_IRQ_Type IRQ_EXTI11;
//static FW_EXTI_IRQ_Type IRQ_EXTI12;
//static FW_EXTI_IRQ_Type IRQ_EXTI13;
//static FW_EXTI_IRQ_Type IRQ_EXTI14;
//static FW_EXTI_IRQ_Type IRQ_EXTI15;


//__INLINE_STATIC_ u8 EXTI_IRQHandler(FW_EXTI_IRQ_Type *irq, void *args)
//{
//    u32 pin_num = (u32)args;
//    exti_line_enum exti_line = EXTIx_LINE(pin_num);
//    if(exti_interrupt_flag_get(exti_line) == SET)
//    {
//        FW_EXTI_IH_ISR(irq);
//        exti_interrupt_flag_clear(exti_line);
//        return True;
//    }
//    return False;
//}

//void EXTI0_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI0, 0);
//}

//void EXTI1_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI1, 1);
//}

//void EXTI2_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI2, 2);
//}

//void EXTI3_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI3, 3);
//}

//void EXTI4_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI4, 4);
//}

//void EXTI5_9_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI5, 5);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI6, 6);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI7, 7);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI8, 8);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI9, 9);
//}

//void EXTI10_15_IRQHandler(void)
//{
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI10, 10);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI11, 11);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI12, 12);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI13, 13);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI14, 14);
//    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI15, 15);
//}


//__INLINE_STATIC_ void EXTI_Init(FW_EXTI_Type *pdev)
//{
//    u16 pin = pdev->Pin;
//    exti_trig_type_enum trig;
//    exti_line_enum line;
//    IRQn_Type irqn;
//    
//    line = EXTIx_LINE(pin);
//    irqn = EXTIx_IRQn(pin);
//    
//    trig = (pdev->Trigger == FW_EXTI_Trigger_Falling) ? EXTI_TRIG_FALLING :\
//           (pdev->Trigger == FW_EXTI_Trigger_Rising) ? EXTI_TRIG_RISING :\
//           (pdev->Trigger == FW_EXTI_Trigger_Both) ? EXTI_TRIG_BOTH :\
//           (exti_trig_type_enum)INVALUE;
//    
//    /* 选择外部中断线 */
//    gpio_exti_source_select(GPIO_PORTx_SOURCE(pin), GPIO_PINx_SOURCE(pin));
//    
//    /* 默认最低中断优先级 */
//    nvic_irq_enable(irqn, 3, 3);
//    
//    /* 设置外部触发线 */
//    exti_init(line, EXTI_INTERRUPT, trig);
//    exti_interrupt_flag_clear(line);
//}

//__INLINE_STATIC_ void EXTI_CTL(FW_EXTI_Type *pdev, u8 state)
//{
//    state ?\
//    exti_interrupt_enable(EXTIx_LINE(pdev->Pin)) :\
//    exti_interrupt_disable(EXTIx_LINE(pdev->Pin));
//}

//__INLINE_STATIC_ FW_EXTI_IRQ_Type *EXTI_Attach_IRQ(FW_EXTI_Type *dev)
//{
//    u16 pin_num = dev->Pin & PIN_MASK;
//    if(pin_num == 0)  return &IRQ_EXTI0;
//    if(pin_num == 1)  return &IRQ_EXTI1;
//    if(pin_num == 2)  return &IRQ_EXTI2;
//    if(pin_num == 3)  return &IRQ_EXTI3;
//    if(pin_num == 4)  return &IRQ_EXTI4;
//    if(pin_num == 5)  return &IRQ_EXTI5;
//    if(pin_num == 6)  return &IRQ_EXTI6;
//    if(pin_num == 7)  return &IRQ_EXTI7;
//    if(pin_num == 8)  return &IRQ_EXTI8;
//    if(pin_num == 9)  return &IRQ_EXTI9;
//    if(pin_num == 10)  return &IRQ_EXTI10;
//    if(pin_num == 11)  return &IRQ_EXTI11;
//    if(pin_num == 12)  return &IRQ_EXTI12;
//    if(pin_num == 13)  return &IRQ_EXTI13;
//    if(pin_num == 14)  return &IRQ_EXTI14;
//    if(pin_num == 15)  return &IRQ_EXTI15;
//    return NULL;
//}




///* EXTI Driver */
//__CONST_STATIC_ FW_EXTI_Driver_Type EXTI_Driver =
//{
//    .Init = EXTI_Init,
//    .CTL = EXTI_CTL,
//    .Attach_IRQ = EXTI_Attach_IRQ,
//};
//FW_DRIVER_REGIST("ll->exti", &EXTI_Driver, EXTI);

