#include "ll_gpio.h"

#include "fw_gpio.h"



/*
fw_gpio.h与swm181.h中对PORT[A:G]都有定义，编译时会出现警告。fw_gpio.h属于框架的
公共库，一般情况下是不能随意变动的，但厂家的固件库也轻易不要变动。对于该问题，由
于应用层不会调用底层(无关联)，底层也未使用fw_gpio.h中的PORT[A:G],所以两者实际并不
会出现冲突。对于此类警告，忽略即可。
*/


__INLINE_STATIC_ void Pin_Init(FW_GPIO_Type *gpio, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    u8 dir, pull_up, pull_down, open_drain;
    
    switch(mode)
    {
        //模拟输入
        case FW_GPIO_Mode_AIN:                   //模拟输入
            dir = 0, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
        
        //通用IO输入
        case FW_GPIO_Mode_IPN:                   //浮空输入
            dir = 0, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_IPU:                   //上拉输入
            dir = 0, pull_up = 1, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_IPD:                   //下拉输入
            dir = 0, pull_up = 0, pull_down = 1, open_drain = 0;
            break;
        
        //外设输入
        case FW_GPIO_Mode_AF_IPN:                //浮空输入
            dir = 0, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_AF_IPU:                //上拉输入
            dir = 0, pull_up = 1, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_AF_IPD:                //下拉输入
            dir = 0, pull_up = 0, pull_down = 1, open_drain = 0;
            break;
        
        //模拟输出
        case FW_GPIO_Mode_AOUT:                  //模拟输出
            dir = 1, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
        
        //通用IO输出
        case FW_GPIO_Mode_Out_PPN:               //浮空推挽输出
            dir = 1, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_Out_PPU:               //上拉推挽输出
            dir = 1, pull_up = 1, pull_down = 0, open_drain = 0;
            break;
        
        case FW_GPIO_Mode_Out_PPD:               //下拉推挽输出
            dir = 1, pull_up = 0, pull_down = 1, open_drain = 0;
            break;
			
		case FW_GPIO_Mode_Out_ODN:               //浮空开漏输出
            dir = 1, pull_up = 0, pull_down = 0, open_drain = 1;
            break;
			
		case FW_GPIO_Mode_Out_ODU:               //浮空上拉输出
            dir = 1, pull_up = 1, pull_down = 0, open_drain = 1;
            break;
			
		case FW_GPIO_Mode_Out_ODD:               //浮空下拉输出
            dir = 1, pull_up = 0, pull_down = 1, open_drain = 1;
            break;
        
        //外设输出			
		case FW_GPIO_Mode_AF_Out_PPN:            //复用推挽浮空输出
            dir = 1, pull_up = 0, pull_down = 0, open_drain = 0;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPU:            //复用推挽上拉输出
            dir = 1, pull_up = 1, pull_down = 0, open_drain = 0;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPD:            //复用推挽下拉输出
            dir = 1, pull_up = 0, pull_down = 1, open_drain = 0;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODN:            //复用开漏浮空输出
            dir = 1, pull_up = 0, pull_down = 0, open_drain = 1;
            break;
		
		case FW_GPIO_Mode_AF_Out_ODU:            //复用开漏上拉输出
            dir = 1, pull_up = 1, pull_down = 0, open_drain = 1;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODD:            //复用开漏下拉输出
            dir = 1, pull_up = 0, pull_down = 1, open_drain = 1;
            break;
			
		default:
            break;
    }
    
    GPIO_Init(GPIOx(pin), PINx(pin), dir, pull_up, pull_down, open_drain);
}

__INLINE_STATIC_ void Pin_Write(FW_GPIO_Type *gpio, u16 pin, u8 value)
{
    (value) ?\
    GPIO_SetBit(GPIOx(pin), PINx(pin)) :\
    GPIO_ClrBit(GPIOx(pin), PINx(pin));
}

__INLINE_STATIC_ u8   Pin_GetOutput(FW_GPIO_Type *gpio, u16 pin)
{
    return (u8)GPIO_GetBit(GPIOx(pin), PINx(pin));
}

__INLINE_STATIC_ u8   Pin_Read(FW_GPIO_Type *gpio, u16 pin)
{
    return (u8)GPIO_GetBit(GPIOx(pin), PINx(pin));
}

__INLINE_STATIC_ void Pin_Toggle(FW_GPIO_Type *gpio, u16 pin)
{
    GPIO_InvBit(GPIOx(pin), PINx(pin));
}

//__INLINE_STATIC_ void Port_DeInit(u16 port)
//{
//    gpio_deinit(GPIOx(port));
//}

__INLINE_STATIC_ void Port_Write(FW_GPIO_Type *gpio, u16 port, u32 value)
{
    (GPIOx(port))->DATA = value;
}

__INLINE_STATIC_ u32  Port_GetOutput(FW_GPIO_Type *gpio, u16 port)
{
    return (u32)((GPIOx(port))->DATA);
}

__INLINE_STATIC_ u32  Port_Read(FW_GPIO_Type *gpio, u16 port)
{
    return (u32)((GPIOx(port))->DATA);
}




/* GPIO Driver */
__CONST_STATIC_ FW_GPIO_Driver_Type HGPIO_Driver =
{
//    .Pin_DeInit      = Pin_DeInit,
    .Pin_Init        = Pin_Init,
    .Pin_Write       = Pin_Write,
    .Pin_GetOutput   = Pin_GetOutput,
    .Pin_Read        = Pin_Read,
    .Pin_Toggle      = NULL,
    
//    .Port_GetDefault = Port_GetDefault,
//    .Port_DeInit     = Port_DeInit,
    .Port_Init       = NULL,
    .Port_Write      = Port_Write,
    .Port_GetOutput  = Port_GetOutput,
    .Port_Read       = Port_Read,
};
FW_DRIVER_REGIST("ll->gpio", &HGPIO_Driver, HGPIO);




#include "project_debug.h"
#if MODULE_TEST && GPIO_TEST
#include "fw_delay.h"


void Test(void)
{
    u16 ve = PD1;
    u16 led = PB4;
    u16 key = PA0;
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

