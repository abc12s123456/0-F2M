#include "ll_gpio.h"

#include "fw_gpio.h"


//#define RCU_GPIOx(pin)(\
//(GPIOx(pin) == GPIOA) ? RCU_GPIOA :\
//(GPIOx(pin) == GPIOB) ? RCU_GPIOB :\
//(GPIOx(pin) == GPIOC) ? RCU_GPIOC :\
//(GPIOx(pin) == GPIOD) ? RCU_GPIOD :\
//(GPIOx(pin) == GPIOE) ? RCU_GPIOE :\
//(GPIOx(pin) == GPIOF) ? RCU_GPIOF :\
//(GPIOx(pin) == GPIOG) ? RCU_GPIOG :\
//(rcu_periph_enum)INVALUE)
#define CRM_GPIOx_PERIPH_CLOCK(pin)(\
(GPIOx(pin) == GPIOA) ? CRM_GPIOA_PERIPH_CLOCK :\
(GPIOx(pin) == GPIOB) ? CRM_GPIOB_PERIPH_CLOCK :\
(GPIOx(pin) == GPIOC) ? CRM_GPIOC_PERIPH_CLOCK :\
(GPIOx(pin) == GPIOD) ? CRM_GPIOD_PERIPH_CLOCK :\
(GPIOx(pin) == GPIOE) ? CRM_GPIOE_PERIPH_CLOCK :\
(crm_periph_clock_type)INVALUE)


//#define GPIO_PORT_SOURCE_GPIOx(pin)(\
//(GPIOx(pin) == GPIOA) ? GPIO_PORT_SOURCE_GPIOA :\
//(GPIOx(pin) == GPIOB) ? GPIO_PORT_SOURCE_GPIOB :\
//(GPIOx(pin) == GPIOC) ? GPIO_PORT_SOURCE_GPIOC :\
//(GPIOx(pin) == GPIOD) ? GPIO_PORT_SOURCE_GPIOD :\
//(GPIOx(pin) == GPIOE) ? GPIO_PORT_SOURCE_GPIOE :\
//(GPIOx(pin) == GPIOF) ? GPIO_PORT_SOURCE_GPIOF :\
//(GPIOx(pin) == GPIOG) ? GPIO_PORT_SOURCE_GPIOG :\
//INVALUE)


__INLINE_STATIC_ void Pin_DeInit(FW_GPIO_Type *dev, u16 pin)
{
    gpio_reset(GPIOx(pin));
}

__INLINE_STATIC_ void Pin_Init(FW_GPIO_Type *dev, u16 pin, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    gpio_init_type gpio_init_struct;

    crm_periph_clock_enable(CRM_GPIOx_PERIPH_CLOCK(pin), TRUE);
    
    gpio_default_para_init(&gpio_init_struct);
    
    switch(mode)
    {
        //模拟输入
        case FW_GPIO_Mode_AIN:                   //模拟输入
            gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
        
        //通用IO输入
        case FW_GPIO_Mode_IPN:                   //浮空输入
            gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
        
        case FW_GPIO_Mode_IPU:                   //上拉输入
            gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_IPD:                   //下拉输入
            gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
        
        //外设输入
        case FW_GPIO_Mode_AF_IPN:                //浮空输入
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
        
        case FW_GPIO_Mode_AF_IPU:                //上拉输入
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_AF_IPD:                //下拉输入
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
        
        //模拟输出
        case FW_GPIO_Mode_AOUT:                  //模拟输出
            gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
        
        //通用IO输出
        case FW_GPIO_Mode_Out_PPN:               //浮空推挽输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
        
        case FW_GPIO_Mode_Out_PPU:               //上拉推挽输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
        
        case FW_GPIO_Mode_Out_PPD:               //下拉推挽输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
			
		case FW_GPIO_Mode_Out_ODN:               //浮空开漏输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
			
		case FW_GPIO_Mode_Out_ODU:               //浮空上拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_Out_ODD:               //浮空下拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
        
        //外设输出			
		case FW_GPIO_Mode_AF_Out_PPN:            //复用推挽浮空输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPU:            //复用推挽上拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_AF_Out_PPD:            //复用推挽下拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODN:            //复用开漏浮空输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
            break;
		
		case FW_GPIO_Mode_AF_Out_ODU:            //复用开漏上拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_UP;
            break;
			
		case FW_GPIO_Mode_AF_Out_ODD:            //复用开漏下拉输出
            gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
            gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
            gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
            gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
            break;
			
		default:
            break;
    }
    
    gpio_init_struct.gpio_pins = GPIO_PINx(pin);
    
    gpio_init(GPIOx(pin), &gpio_init_struct);
}

__INLINE_STATIC_ void Pin_Write(FW_GPIO_Type *dev, u16 pin, u8 value)
{
    (value) ?\
    gpio_bits_set(GPIOx(pin), GPIO_PINx(pin)) :\
    gpio_bits_reset(GPIOx(pin), GPIO_PINx(pin));
}

__INLINE_STATIC_ u8   Pin_GetOutput(FW_GPIO_Type *dev, u16 pin)
{
    return (u8)gpio_output_data_bit_read(GPIOx(pin), GPIO_PINx(pin));
}

__INLINE_STATIC_ u8   Pin_Read(FW_GPIO_Type *dev, u16 pin)
{
    return (u8)gpio_input_data_bit_read(GPIOx(pin), GPIO_PINx(pin));
}

__INLINE_STATIC_ void Port_GetDefault(FW_GPIO_Type *dev)
{
    dev->Default_Mode = FW_GPIO_Mode_AOUT;
    dev->Default_Speed = FW_GPIO_Speed_Low;
}

__INLINE_STATIC_ void Port_DeInit(FW_GPIO_Type *dev, u16 port)
{
    
}

__INLINE_STATIC_ void Port_Write(FW_GPIO_Type *dev, u16 port, u32 value)
{
    gpio_port_write(GPIOx(port), (u16)value);
}

__INLINE_STATIC_ u32  Port_GetOutput(FW_GPIO_Type *dev, u16 port)
{
    return (u32)gpio_output_data_read(GPIOx(port));
}

__INLINE_STATIC_ u32  Port_Read(FW_GPIO_Type *dev, u16 port)
{
    return (u32)gpio_input_data_read(GPIOx(port));
}



/* GPIO Driver */
__CONST_STATIC_ FW_GPIO_Driver_Type HGPIO_Driver =
{
    .Pin_DeInit      = Pin_DeInit,
    .Pin_Init        = Pin_Init,
    .Pin_Write       = Pin_Write,
    .Pin_GetOutput   = Pin_GetOutput,
    .Pin_Read        = Pin_Read,
    .Pin_Toggle      = NULL,
    
    .Port_GetDefault = Port_GetDefault,
    .Port_DeInit     = Port_DeInit,
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

#elif MODULE_TEST && GPIO_TEST
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

