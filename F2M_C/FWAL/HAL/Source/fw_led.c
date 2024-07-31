#include "fw_led.h"
#include "fw_gpio.h"
#include "fw_delay.h"


void FW_LED_Init(FW_LED_Type *pdev)
{
    u16 pin = pdev->Pin;
    
    /* 软定时器 */
    if(pdev->Timing)
    {
        if(pdev->Timing->Get_Tick == NULL)
        {
            STiming_Init(pdev->Timing, NULL);
        }
        else
        {
            STiming_Init(pdev->Timing, pdev->Timing->Get_Tick);
        }
    }
    
    /* IO初始化 */
    if(pdev->Level)
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    }
    else
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    }
    
    /* LED初始化完成后，默认关闭 */
    FW_GPIO_Write(pin, !pdev->Level);
    pdev->State = OFF;
}

void FW_LED_SetPin(FW_LED_Type *pdev, u16 pin)
{
    pdev->Pin = pin;
}

u16  FW_LED_GetPin(FW_LED_Type *pdev)
{
    return pdev->Pin;
}
void FW_LED_SetLevel(FW_LED_Type *pdev, u8 level)
{
    pdev->Level = level;
}
u8   FW_LED_GetLevel(FW_LED_Type *pdev)
{
    return pdev->Level;
}

void FW_LED_ON(FW_LED_Type *pdev)
{
    FW_GPIO_Write(pdev->Pin, pdev->Level);
    pdev->State = ON;
}
void FW_LED_OFF(FW_LED_Type *pdev)
{
    FW_GPIO_Write(pdev->Pin, !pdev->Level);
    pdev->State = OFF;
}

u8   FW_LED_GetState(FW_LED_Type *pdev)
{
    return pdev->State;
}

void FW_LED_Shine(FW_LED_Type *pdev, u16 period, u8 times)
{
    period >>= 1;
    while(times--)
    {
        FW_LED_ON(pdev);
        FW_Delay_Ms(period);
        FW_LED_OFF(pdev);
        FW_Delay_Ms(period);
    }
}

void FW_LED_Blink(FW_LED_Type *pdev, u16 period)
{
    if(STiming_Start(pdev->Timing, period >> 1) == True)
    {
        (pdev->State) ? FW_LED_OFF(pdev) : FW_LED_ON(pdev);
    }
}

void FW_LED_AdjBlink(FW_LED_Type *pdev, u16 period, u8 dc)
{
    u16 tmp;
    
    if(dc > 100)  dc = 100;
    
    /* LED打开后，等待period * dc / 100后关闭 */
    if(pdev->State)
    {
        tmp = period * dc / 100;
        if(STiming_Start(pdev->Timing, tmp) == True)
        {
            FW_LED_OFF(pdev);
            STiming_Reset(pdev->Timing);
        }
    }
    else
    {
        tmp = period * (100 - dc) / 100;
        if(STiming_Start(pdev->Timing, tmp) == True)
        {
            FW_LED_ON(pdev);
            STiming_Reset(pdev->Timing);
        }
    }
}


#define LED_BREATH_PERIOD    20        //呼吸周期，单位：ms


void FW_LED_Breath(FW_LED_Type *pdev, u16 period)
{
    u16 times, offset;
    u16 i, j = 0;
    u16 pin = pdev->Pin;
    u8 level = pdev->Level;
    
    /* period >> 2的目的是一个呼吸周期为暗->亮->暗，而暗->亮及亮->暗的空档，还需
       要一个相同时间的延迟;
       j为占空比值 */
    period >>= 2;
    
    while((period) > (++j * LED_BREATH_PERIOD * LED_BREATH_PERIOD));
    times = j * LED_BREATH_PERIOD;
    offset = j;
    j = 0;
    
    /* 由暗至亮 */
    for(i = 1; i < times; i++)
    {
        FW_GPIO_Write(pin, !level);
        FW_Delay_Ms(LED_BREATH_PERIOD - j);
        FW_GPIO_Write(pin, level);
        FW_Delay_Ms(j);
        if((i % offset) == 0)  j++;
    }
    
    /* 控制呼吸灯的周期 */
    FW_Delay_Ms(period);
    
    /* 由亮至暗 */
    j = 0;
    for(i = 1; i < times; i++)
    {
        FW_GPIO_Write(pin, level);
        FW_Delay_Ms(LED_BREATH_PERIOD - j);
        FW_GPIO_Write(pin, !level);
        FW_Delay_Ms(j);
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
}

void FW_LED_ExqBreath(FW_LED_Type *pdev, u16 period)
{
    u32 times, offset;
    u32 i, j = 0, k;
    u16 pin = pdev->Pin;
    u8 level = pdev->Level;
    
    /* period >> 2的目的是一个呼吸周期为暗->亮->暗，而暗->亮及亮->暗的空档，还需
       要一个相同时间的延迟;
       j为占空比值 */
    period >>= 2;
    
    while((period) > (++j * LED_BREATH_PERIOD * LED_BREATH_PERIOD));
    times = j * LED_BREATH_PERIOD;
    offset = j;
    j = 0;
    
    for(i = 1; i < times; i++)
    {
        for(k = 0; k < 900; k++)
        {
            FW_GPIO_Write(pin, !level);
            FW_Delay_Us(LED_BREATH_PERIOD - j);
            FW_GPIO_Write(pin, level);
            FW_Delay_Us(j);
        }
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
    
    j = 0;
    for(i = 0; i < times; i++)
    {
        for(k = 0; k < 900; k++)
        {
            FW_GPIO_Write(pin, level);
            FW_Delay_Us(LED_BREATH_PERIOD - j);
            FW_GPIO_Write(pin, !level);
            FW_Delay_Us(j);
        }
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
}




#include "project_config.h"
#if MODULE_TEST && LED_TEST
#include "fw_system.h"


#define led   FW_DEVICE_ID(FW_DT_GPIO, 100, 0, 0)
static FW_LED_Type LED;

static STiming_Type LED_Timing;

static void LED_Config(void *pdata)
{
    LED.Pin = PD3;
    LED.Level = LEVEL_L;
    LED.Timing = &LED_Timing;
}

FW_DEVICE_REGISTER(led, &LED, LED_Config, LED);


void Test(void)
{
    FW_LED_Type *fd;
    u16 VCC_EN = PC13;
    
    FW_System_Init();
    FW_Delay_Init();
    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_CLR(VCC_EN);
    
    fd = FW_Device_Find(led);
    
    FW_LED_Init(fd);
    
    while(1)
    {
        FW_LED_ExqBreath(fd, 2000);
//        FW_LED_AdjBlink(fd, 2000, 10);
    }
}


#endif

