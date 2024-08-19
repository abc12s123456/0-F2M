#include "fw_gpio.h"

#include "ll_gpio.h"




#define LL_AHB1_GRP1_PERIPH_GPIOx(pin)(\
(GPIOx(pin) == GPIOA) ? LL_AHB1_GRP1_PERIPH_GPIOA :\
(GPIOx(pin) == GPIOB) ? LL_AHB1_GRP1_PERIPH_GPIOB :\
(GPIOx(pin) == GPIOC) ? LL_AHB1_GRP1_PERIPH_GPIOC :\
(GPIOx(pin) == GPIOD) ? LL_AHB1_GRP1_PERIPH_GPIOD :\
(GPIOx(pin) == GPIOE) ? LL_AHB1_GRP1_PERIPH_GPIOE :\
(GPIOx(pin) == GPIOH) ? LL_AHB1_GRP1_PERIPH_GPIOH :\
INVALUE)


//#define GPIO_PORT_SOURCE_GPIOx(pin)(\
//(GPIOx(pin) == GPIOA) ? GPIO_PORT_SOURCE_GPIOA :\
//(GPIOx(pin) == GPIOB) ? GPIO_PORT_SOURCE_GPIOB :\
//(GPIOx(pin) == GPIOC) ? GPIO_PORT_SOURCE_GPIOC :\
//(GPIOx(pin) == GPIOD) ? GPIO_PORT_SOURCE_GPIOD :\
//(GPIOx(pin) == GPIOE) ? GPIO_PORT_SOURCE_GPIOE :\
//(GPIOx(pin) == GPIOF) ? GPIO_PORT_SOURCE_GPIOF :\
//(GPIOx(pin) == GPIOG) ? GPIO_PORT_SOURCE_GPIOG :\
//INVALUE)


void LL_GPIO_PinAFConfig(u16 pin, u8 GPIO_AF)
{
    if(pin == PIN_NULL)  return;
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOx(pin));
    if(GPIO_PinSource_x(pin) >= 0x08)  LL_GPIO_SetAFPin_8_15(GPIOx(pin), GPIO_Pin_x(pin), GPIO_AF);
    else  LL_GPIO_SetAFPin_0_7(GPIOx(pin), GPIO_Pin_x(pin), GPIO_AF);
}


__INLINE_STATIC_ void Pin_DeInit(FW_GPIO_Type *dev, u16 pin)
{
    LL_GPIO_DeInit(GPIOx(pin));
}

__INLINE_STATIC_ void Pin_Init(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    u32 gpio_mode, gpio_output, gpio_pull, gpio_speed;
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOx(pin));
    
    switch(mode)
    {
        //模拟输入
        case FW_GPIO_Mode_AIN:                   //模拟输入
            gpio_mode = LL_GPIO_MODE_ANALOG;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
        
        //通用IO输入
        case FW_GPIO_Mode_IPN:                   //浮空输入
            gpio_mode = LL_GPIO_MODE_INPUT;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
        
        case FW_GPIO_Mode_IPU:                   //上拉输入
            gpio_mode = LL_GPIO_MODE_INPUT;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_IPD:                   //下拉输入
            gpio_mode = LL_GPIO_MODE_INPUT;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
        
        //外设输入
        case FW_GPIO_Mode_AF_IPN:                //浮空输入
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
        
        case FW_GPIO_Mode_AF_IPU:                //上拉输入
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_AF_IPD:                //下拉输入
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
        
        //模拟输出
        case FW_GPIO_Mode_AOUT:                  //模拟输出
            gpio_mode = LL_GPIO_MODE_ANALOG;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
        
        //通用IO输出
        case FW_GPIO_Mode_Out_PPN:               //浮空推挽输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
        
        case FW_GPIO_Mode_Out_PPU:               //上拉推挽输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_Out_PPD:               //下拉推挽输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
			
		case FW_GPIO_Mode_Out_ODN:               //浮空开漏输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
			
		case FW_GPIO_Mode_Out_ODU:               //浮空上拉输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_Out_ODD:               //浮空下拉输出
            gpio_mode = LL_GPIO_MODE_OUTPUT;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
        
        //外设输出			
		case FW_GPIO_Mode_AF_Out_PPN:            //复用推挽浮空输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPU:            //复用推挽上拉输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPD:            //复用推挽下拉输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_PUSHPULL;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODN:            //复用开漏浮空输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_NO;
            break;
		
		case FW_GPIO_Mode_AF_Out_ODU:            //复用开漏上拉输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODD:            //复用开漏下拉输出
            gpio_mode = LL_GPIO_MODE_ALTERNATE;
            gpio_output = LL_GPIO_OUTPUT_OPENDRAIN;
            gpio_pull = LL_GPIO_PULL_DOWN;
            break;
			
		default:
            break;
    }

    switch(speed)
    {
        case FW_GPIO_Speed_Low:
            gpio_speed = LL_GPIO_SPEED_FREQ_LOW;
            break;
        
        case FW_GPIO_Speed_Medium:
            gpio_speed = LL_GPIO_SPEED_FREQ_MEDIUM;
            break;
        
        case FW_GPIO_Speed_High:
            gpio_speed = LL_GPIO_SPEED_FREQ_HIGH;
            break;
        
        case FW_GPIO_Speed_Ultra:
            gpio_speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            break;
        
        default:
            break;
    }
    
    LL_GPIO_InitTypeDef gpio_initstruct;
    
    gpio_initstruct.Pin        = GPIO_Pin_x(pin);
    gpio_initstruct.Mode       = LL_GPIO_MODE_OUTPUT;
    gpio_initstruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    gpio_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_initstruct.Pull       = LL_GPIO_PULL_NO;
    
    LL_GPIO_Init(GPIOx(pin), &gpio_initstruct);
}

__INLINE_STATIC_ void Pin_Write(FW_GPIO_Type *dev, u16 pin, u8 value)
{
    (value) ?\
    LL_GPIO_SetOutputPin(GPIOx(pin), GPIO_Pin_x(pin)) :\
    LL_GPIO_ResetOutputPin(GPIOx(pin), GPIO_Pin_x(pin));
}

__INLINE_STATIC_ u8   Pin_GetOutput(FW_GPIO_Type *dev, u16 pin)
{
    return LL_GPIO_IsOutputPinSet(GPIOx(pin), GPIO_Pin_x(pin));
}

__INLINE_STATIC_ u8   Pin_Read(FW_GPIO_Type *dev, u16 pin)
{
    return LL_GPIO_IsInputPinSet(GPIOx(pin), GPIO_Pin_x(pin));
}

__INLINE_STATIC_ void Pin_Toggle(FW_GPIO_Type *dev, u16 pin)
{
    LL_GPIO_TogglePin(GPIOx(pin), GPIO_Pin_x(pin));
}

__INLINE_STATIC_ void Port_GetDefault(FW_GPIO_Type *dev)
{
    dev->Default_Mode = FW_GPIO_Mode_AOUT;
    dev->Default_Speed = FW_GPIO_Speed_Low;
}

__INLINE_STATIC_ void Port_DeInit(FW_GPIO_Type *dev, u16 port)
{
    LL_GPIO_DeInit(GPIOx(port));
}

__INLINE_STATIC_ void Port_Write(FW_GPIO_Type *dev, u16 port, u32 value)
{
    LL_GPIO_WriteOutputPort(GPIOx(port), value);
}

__INLINE_STATIC_ u32  Port_GetOutput(FW_GPIO_Type *dev, u16 port)
{
    return LL_GPIO_ReadOutputPort(GPIOx(port));
}

__INLINE_STATIC_ u32  Port_Read(FW_GPIO_Type *dev, u16 port)
{
    return LL_GPIO_ReadInputPort(GPIOx(port));
}



/* GPIO Driver */
__CONST_STATIC_ FW_GPIO_Driver_Type HGPIO_Driver =
{
    .Pin_DeInit      = Pin_DeInit,
    .Pin_Init        = Pin_Init,
    .Pin_Write       = Pin_Write,
    .Pin_GetOutput   = Pin_GetOutput,
    .Pin_Read        = Pin_Read,
    .Pin_Toggle      = Pin_Toggle,
    
    .Port_GetDefault = Port_GetDefault,
    .Port_DeInit     = Port_DeInit,
    .Port_Init       = NULL,
    .Port_Write      = Port_Write,
    .Port_GetOutput  = Port_GetOutput,
    .Port_Read       = Port_Read,
};
FW_DRIVER_REGIST("ll->gpio", &HGPIO_Driver, HGPIO);




#include "fw_debug.h"
#if MODULE_TEST && GPIO_TEST
#include "fw_delay.h"


void Test(void)
{
    u16 ve = PD1;
    u16 led = PA0;
    u16 key = PA1;
    u32 cnt = 500;
    
    FW_GPIO_Init(ve, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(led, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(key, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(ve);
    
    while(1)
    {
        if(FW_GPIO_Read(key) == LEVEL_H)
        {
            cnt += 500;
            if(cnt >= 2000)  cnt = 500;
        }
        
        FW_GPIO_SET(led);
        FW_Delay_Ms(cnt);
        FW_GPIO_CLR(led);
        FW_Delay_Ms(cnt);
    }
}

#elif MODULE_TEST && IR_TEST
#include "fw_delay.h"


void Test(void)
{
    u16 VCC_EN = PD1;
    u16 IR_IN = PA1;
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(IR_IN, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(VCC_EN);
    
    while(1)
    {
        if(FW_GPIO_Read(IR_IN) == LEVEL_L)
        {
            FW_Delay_Ms(100);
            if(FW_GPIO_Read(IR_IN) == LEVEL_L)
            {
                while(0);
            }
        }
        FW_Delay_Ms(10);
    }
}

#endif

