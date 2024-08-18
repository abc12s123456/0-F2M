#include "ll_gpio.h"

#include "fw_gpio.h"


//#define LL_IOP_GRP1_PERIPH_GPIOx(pin)(\
//(GPIOx(pin) == GPIOA) ? LL_IOP_GRP1_PERIPH_GPIOA :\
//(GPIOx(pin) == GPIOB) ? LL_IOP_GRP1_PERIPH_GPIOB :\
//(GPIOx(pin) == GPIOF) ? LL_IOP_GRP1_PERIPH_GPIOF :\
//INVALUE)
#define LL_IOP_GRP1_PERIPH_GPIOx(pin)  (1 << ((pin >> 8) - 1))


void LL_GPIO_PinAFConfig(u16 pin, u8 GPIO_AF)
{
    if(pin == PIN_NULL)  return;
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOx(pin));
    if(GPIO_PinSource_x(pin) >= 0x08)  LL_GPIO_SetAFPin_8_15(GPIOx(pin), GPIO_Pin_x(pin), GPIO_AF);
    else  LL_GPIO_SetAFPin_0_7(GPIOx(pin), GPIO_Pin_x(pin), GPIO_AF);
}


__INLINE_STATIC_ void Pin_DeInit(FW_GPIO_Type *dev, u16 pin)
{
    LL_GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    GPIO_InitStructure.Pin = GPIO_Pin_x(pin);
    GPIO_InitStructure.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_LOW;
    
    LL_GPIO_Init(GPIOx(pin), &GPIO_InitStructure);
}

__INLINE_STATIC_ void Pin_Init(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    if(pin == PIN_NULL)  return;
    
    LL_GPIO_InitTypeDef GPIO_InitStructure = {0};
    u8 LL_GPIO_AF_x;
    
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOx(pin));
    
    if(GPIO_PinSource_x(pin) >= 0x08)
    {
        LL_GPIO_AF_x = LL_GPIO_GetAFPin_8_15(GPIOx(pin), GPIO_Pin_x(pin));
    }
    else
    {
        LL_GPIO_AF_x = LL_GPIO_GetAFPin_0_7(GPIOx(pin), GPIO_Pin_x(pin));
    }
    
    GPIO_InitStructure.Pin = GPIO_Pin_x(pin);
    switch(mode)
    {
        //模拟输入
        case FW_GPIO_Mode_AIN:                         //模拟输入输出
        case FW_GPIO_Mode_AOUT:
            GPIO_InitStructure.Mode = LL_GPIO_MODE_ANALOG;
            GPIO_InitStructure.Pull = LL_GPIO_OUTPUT_OPENDRAIN;
            break;
        
        //通用IO输入
        case FW_GPIO_Mode_IPN:                         //浮空输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_INPUT;            
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;
            break;
        
        case FW_GPIO_Mode_IPU:                         //上拉输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_INPUT;    
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_IPD:                         //下拉输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_INPUT;          
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN;
            break;
        
        //外设输入
        case FW_GPIO_Mode_AF_IPN:                      //浮空输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;      
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
        
        case FW_GPIO_Mode_AF_IPU:                      //上拉输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;      
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
        
        case FW_GPIO_Mode_AF_IPD:                      //下拉输入
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;      
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
        
        //通用IO输出
        case FW_GPIO_Mode_Out_PPN:                     //浮空推挽输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;   
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;  
            break;
        
        case FW_GPIO_Mode_Out_PPU:                     //上拉推挽输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;   
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP; 
            break;
        
        case FW_GPIO_Mode_Out_PPD:                     //下拉推挽输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;   
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN; 
            break;
			
		case FW_GPIO_Mode_Out_ODN:                     //浮空开漏输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;
            break;
			
		case FW_GPIO_Mode_Out_ODU:                     //浮空上拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP; 
            break;
			
		case FW_GPIO_Mode_Out_ODD:                     //浮空下拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_OUTPUT;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN; 
            break;
        
        //外设输出			
		case FW_GPIO_Mode_AF_Out_PPN:                  //复用推挽浮空输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPU:                  //复用推挽上拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPD:                  //复用推挽下拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODN:                  //复用开漏浮空输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_NO;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x; 
            break;
		
		case FW_GPIO_Mode_AF_Out_ODU:                  //复用开漏上拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_UP;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x; 
            break;
			
		case FW_GPIO_Mode_AF_Out_ODD:                  //复用开漏下拉输出
            GPIO_InitStructure.Mode  = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
            GPIO_InitStructure.Pull  = LL_GPIO_PULL_DOWN;
            GPIO_InitStructure.Alternate = LL_GPIO_AF_x;
            break;
			
		default:
            break;
    }
    
    switch(speed)
    {
        case FW_GPIO_Speed_Low:
            GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_LOW;
            break;

        case FW_GPIO_Speed_Medium:
            GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
            break;

        case FW_GPIO_Speed_High:
            GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_HIGH;
            break;
        
        case FW_GPIO_Speed_Ultra:
            GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            break;
        
        default:
            GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_LOW;
            break;
    }
    
    LL_GPIO_Init(GPIOx(pin), &GPIO_InitStructure);
}

__INLINE_STATIC_ void Pin_Write(FW_GPIO_Type *dev, u16 pin, u8 value)
{
    value ?
    LL_GPIO_SetOutputPin(GPIOx(pin), GPIO_Pin_x(pin)) :
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

__INLINE_STATIC_ void Port_DeInit(FW_GPIO_Type *dev, u16 port)
{
    
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
#define HGPIO_DRV_ID  FW_DRIVER_ID(FW_Device_GPIO, GPIO_NATIVE_DRV_NUM)

const static FW_GPIO_Driver_Type HGPIO_Driver =
{
    .Pin_DeInit    = Pin_DeInit,
    .Pin_Init      = Pin_Init,
    .Pin_Write     = Pin_Write,
    .Pin_GetOutput = Pin_GetOutput,
    .Pin_Read      = Pin_Read,
    .Pin_Toggle    = Pin_Toggle,
    
    .Port_DeInit    = Port_DeInit,
    .Port_Init      = NULL,
    .Port_Write     = Port_Write,
    .Port_GetOutput = Port_GetOutput,
    .Port_Read      = Port_Read,
};
FW_DRIVER_REGIST("ll->gpio", &HGPIO_Driver, HGPIO);

