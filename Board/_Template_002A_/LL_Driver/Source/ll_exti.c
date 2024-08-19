#include "ll_include.h"
#include "ll_gpio.h"

#include "fw_exti.h"
#include "fw_gpio.h"


#if defined(EXTI_MOD_EN) && EXTI_MOD_EN


//#define LL_EXTI_LINE_x(pin)(\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_0) ? LL_EXTI_LINE_0 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_1) ? LL_EXTI_LINE_1 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_2) ? LL_EXTI_LINE_2 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_3) ? LL_EXTI_LINE_3 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_4) ? LL_EXTI_LINE_4 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_5) ? LL_EXTI_LINE_5 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_6) ? LL_EXTI_LINE_6 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_7) ? LL_EXTI_LINE_7 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_8) ? LL_EXTI_LINE_8 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_9) ? LL_EXTI_LINE_9 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_10) ? LL_EXTI_LINE_10 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_11) ? LL_EXTI_LINE_11 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_12) ? LL_EXTI_LINE_12 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_13) ? LL_EXTI_LINE_13 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_14) ? LL_EXTI_LINE_14 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_15) ? LL_EXTI_LINE_15 :\
//INVALUE)

/* 将应用层的pin映射到LL_EXTI_LINE_0~LL_EXTI_LINE_15 */
#define LL_EXTI_LINE_x(pin)  (1 << ((pin) & 0xFF))

//#define EXTIx_IRQn(pin)(\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_0 || GPIO_Pin_x(pin) == LL_GPIO_PIN_1) ? EXTI0_1_IRQn :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_2 || GPIO_Pin_x(pin) == LL_GPIO_PIN_3) ? EXTI2_3_IRQn :\
//(GPIO_Pin_x(pin) >= LL_GPIO_PIN_4 && GPIO_Pin_x(pin) <= LL_GPIO_PIN_15) ? EXTI4_15_IRQn :\
//(IRQn_Type)INVALUE)
/* 相对于上面的代码，节省了20B的ROM占用 */
__INLINE_STATIC_ IRQn_Type EXTIx_IRQn(u16 pin)
{
    pin &= 0xFF;
    
    if(pin == 0 || pin == 1)  return EXTI0_1_IRQn;
    if(pin == 2 || pin == 3)  return EXTI2_3_IRQn;
    if(pin >= 4 && pin <= 15)  return EXTI4_15_IRQn;
    
    return (IRQn_Type)INVALUE;
}

//#define LL_EXTI_CONFIG_PORTx(pin)(\
//(GPIOx(pin) == GPIOA) ? LL_EXTI_CONFIG_PORTA :\
//(GPIOx(pin) == GPIOB) ? LL_EXTI_CONFIG_PORTB :\
//(GPIOx(pin) == GPIOF) ? LL_EXTI_CONFIG_PORTF :\
//INVALUE)
__INLINE_STATIC_ u8   LL_EXTI_CONFIG_PORTx(u16 pin)
{
    pin = (pin >> 8) - 1;
    if(pin == 5)  return LL_EXTI_CONFIG_PORTF;
    else  return pin;
}

//#define LL_EXTI_CONFIG_LINEx(pin)(\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_0) ? LL_EXTI_CONFIG_LINE0 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_1) ? LL_EXTI_CONFIG_LINE1 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_2) ? LL_EXTI_CONFIG_LINE2 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_3) ? LL_EXTI_CONFIG_LINE3 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_4) ? LL_EXTI_CONFIG_LINE4 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_5) ? LL_EXTI_CONFIG_LINE5 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_6) ? LL_EXTI_CONFIG_LINE6 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_7) ? LL_EXTI_CONFIG_LINE7 :\
//(GPIO_Pin_x(pin) == LL_GPIO_PIN_8) ? LL_EXTI_CONFIG_LINE8 :\
//INVALUE)
__INLINE_STATIC_ u32  LL_EXTI_CONFIG_LINEx(u16 pin)
{
    u8 a, b, c;
    pin &= 0xFF;
    
    if(pin > 4)  a = 1;
    else  a = 3;
    
    b = (pin % 4) * 8;
    c = pin / 4;
    
    return (u32)((a << LL_EXTI_REGISTER_PINMASK_SHFT) | ( b << LL_EXTI_REGISTER_PINPOS_SHFT) | c);
}


#define IRQ_EXTI_BASE         (0x20000100)
#define IRQ_EXTI_OFFSET(num)  (sizeof(FW_EXTI_IRQ_Type) * (num))
static FW_EXTI_IRQ_Type IRQ_EXTI0 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(0));
static FW_EXTI_IRQ_Type IRQ_EXTI1 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(1));
static FW_EXTI_IRQ_Type IRQ_EXTI2 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(2));
static FW_EXTI_IRQ_Type IRQ_EXTI3 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(3));
static FW_EXTI_IRQ_Type IRQ_EXTI4 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(4));
static FW_EXTI_IRQ_Type IRQ_EXTI5 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(5));
static FW_EXTI_IRQ_Type IRQ_EXTI6 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(6));
static FW_EXTI_IRQ_Type IRQ_EXTI7 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(7));
static FW_EXTI_IRQ_Type IRQ_EXTI8 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(8));
static FW_EXTI_IRQ_Type IRQ_EXTI9 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(9));
static FW_EXTI_IRQ_Type IRQ_EXTI10 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(10));
static FW_EXTI_IRQ_Type IRQ_EXTI11 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(11));
static FW_EXTI_IRQ_Type IRQ_EXTI12 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(12));
static FW_EXTI_IRQ_Type IRQ_EXTI13 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(13));
static FW_EXTI_IRQ_Type IRQ_EXTI14 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(14));
static FW_EXTI_IRQ_Type IRQ_EXTI15 __AT_(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(15));


/* 中断服务函数 */
__INLINE_STATIC_ u8 EXTI_IRQHandler(FW_EXTI_IRQ_Type *irq, void *args)
{
    u32 pin_num = (u32)args;
    u32 exti_line = LL_EXTI_LINE_x(pin_num);
    if(LL_EXTI_IsActiveFlag(exti_line) == SET)
    {
        FW_EXTI_IH_ISR(irq);
        LL_EXTI_ClearFlag(exti_line);
        return True;
    }
    return False;
}

void EXTI0_1_IRQHandler(void)
{
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI0, 0);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI1, 1);
}

void EXTI2_3_IRQHandler(void)
{
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI2, 2);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI3, 3);
}

void EXTI4_15_IRQHandler(void)
{
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI4, 4);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI5, 5);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI6, 6);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI7, 7);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI8, 8);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI9, 9);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI10, 10);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI11, 11);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI12, 12);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI13, 13);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI14, 14);
    FW_EXTI_LIH(EXTI_IRQHandler, &IRQ_EXTI15, 15);
}


__INLINE_STATIC_ void EXTI_Init(FW_EXTI_Type *dev)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct;

    u16 pin = dev->Pin;
    u32 trig = 
    (dev->Trigger == FW_EXTI_Trigger_Falling) ? LL_EXTI_TRIGGER_FALLING :
    (dev->Trigger == FW_EXTI_Trigger_Rising) ? LL_EXTI_TRIGGER_FALLING :
    (dev->Trigger == FW_EXTI_Trigger_Both) ? LL_EXTI_TRIGGER_RISING_FALLING :
    INVALUE;
    
    EXTI_InitStruct.Line        = LL_EXTI_LINE_x(pin);
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = trig;
    LL_EXTI_Init(&EXTI_InitStruct);
    
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTx(pin), LL_EXTI_CONFIG_LINEx(pin));
    
    NVIC_SetPriority(EXTIx_IRQn(pin), 3);
    NVIC_EnableIRQ(EXTIx_IRQn(pin));
}

__INLINE_STATIC_ void EXTI_CTL(FW_EXTI_Type *dev, u8 state)
{
    state ?\
    LL_EXTI_EnableIT(LL_EXTI_LINE_x(dev->Pin)) :\
    LL_EXTI_DisableIT(LL_EXTI_LINE_x(dev->Pin));
}

__INLINE_STATIC_ FW_EXTI_IRQ_Type *EXTI_Attach_IRQ(FW_EXTI_Type *dev)
{
    u16 pin = dev->Pin & PIN_MASK;
    return (FW_EXTI_IRQ_Type *)(IRQ_EXTI_BASE + IRQ_EXTI_OFFSET(pin));
}




/* EXTI Driver */
const static FW_EXTI_Driver_Type EXTI_Driver =
{
    .Init = EXTI_Init,
    .CTL = EXTI_CTL,
    .Attach_IRQ = EXTI_Attach_IRQ,
};
FW_DRIVER_REGIST("ll->exti", &EXTI_Driver, EXTI);




#include "project_debug.h"
#if MODULE_TEST && EXTI_TEST
#include "fw_system.h"
#include "fw_delay.h"
#include "led.h"


/*  */
static FW_EXTI_Type EXTI_PA2;

static void EXTI_PA2_Config(void *dev)
{
    EXTI_PA2.Pin = PA2;
    EXTI_PA2.Trigger = FW_EXTI_Trigger_Rising;
    EXTI_PA2.EN = ON;
}
FW_DEVICE_STATIC_REGIST("exti_pa2", &EXTI_PA2, EXTI_PA2_Config, EXTI_PA2);


/*  */
static FW_EXTI_Type EXTI_PC8;

static void EXTI_PC8_Config(void *dev)
{
    EXTI_PC8.Pin = PC8;
    EXTI_PC8.Trigger = FW_EXTI_Trigger_Falling;
    EXTI_PC8.EN = ON;
}
FW_DEVICE_STATIC_REGIST("exti_pc8", &EXTI_PC8, EXTI_PC8_Config, EXTI_PC8);


/*  */
static FW_EXTI_Type EXTI_PC9;

static void EXTI_PC9_Config(void *dev)
{
    EXTI_PC9.Pin = PC9;
    EXTI_PC9.Trigger = FW_EXTI_Trigger_Falling;
    EXTI_PC9.EN = ON;
}
FW_DEVICE_STATIC_REGIST("exti_pc9", &EXTI_PC9, EXTI_PC9_Config, EXTI_PC9);


/*  */
static void IH_CB0(void *pdata)
{
    LED_Type *led = (LED_Type *)pdata;
    
    LED_Shine(led, 500, 2);
}

static void IH_CB8(void *pdata)
{
    LED_Type *led = (LED_Type *)pdata;
    
    LED_Shine(led, 500, 2);
}

static void IH_CB9(void *pdata)
{
    LED_Type *led = (LED_Type *)pdata;
    
    LED_Shine(led, 500, 2);
}


void Test(void)
{
    FW_EXTI_Type *fd;
    LED_Type led1, led2, led3;
    u16 VCC_EN = PC13;
    
    FW_System_Init();
    FW_Delay_Init();
    
    LED_DeInit(&led1);
    led1.Level = LEVEL_L;
    led1.Pin = PA3;
    LED_Init(&led1);
    
//    led2.Level = LEVEL_L;
//    led2.Pin = PD3;
//    LED_Init(&led2);
//    
//    led3.Level = LEVEL_H;
//    led3.Pin = PC4;
//    LED_Init(&led3);
    
    fd = FW_Device_Find("exti_pa2");
    if(fd == NULL)
    {
        while(1);
    }
    
    fd->IH_CB = IH_CB0;
    fd->IH_Pdata = &led1;
    FW_EXTI_Init(fd);
    
//    fd = FW_Device_Find("exti_pc8");
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    fd->IH_CB = IH_CB8;
//    fd->IH_Pdata = &led2;
//    FW_EXTI_Init(fd);
//    
//    fd = FW_Device_Find("exti_pc9");
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    fd->IH_CB = IH_CB9;
//    fd->IH_Pdata = &led3;
//    FW_EXTI_Init(fd);
//    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_SET(VCC_EN);
    
    while(1);
}


#endif

#endif  /* defined(EXTI_MOD_EN) && EXTI_MOD_EN */

