#include "gyy0709.h"

#include "fw_gpio.h"
#include "fw_uart.h"



__INLINE_STATIC_ void WM_IO_Init(WM_Type *dev)
{
    FW_GPIO_Init(dev->L1_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->L2_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->L3_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    FW_GPIO_CLR(dev->L1_Pin);
    FW_GPIO_CLR(dev->L2_Pin);
    FW_GPIO_CLR(dev->L3_Pin);
}

/* 
L1   L2    L3
0    0     x    0     x
0    1     0    2     ==
1    0     0    4     ==
1    1     0    6     
0    1     1    3     
1    0     1    5     
1    1     1    7     === ==

*/
__INLINE_STATIC_ void WM_IO_Output(WM_Type *dev, WM_OM_Enum om)
{
    if(om & 0x04)  FW_GPIO_SET(dev->L1_Pin);
    else  FW_GPIO_CLR(dev->L1_Pin);
    
    if(om & 0x02)  FW_GPIO_SET(dev->L2_Pin);
    else  FW_GPIO_CLR(dev->L2_Pin);
    
    if(om & 0x01)  FW_GPIO_SET(dev->L3_Pin);
    else  FW_GPIO_CLR(dev->L3_Pin);
}


__INLINE_STATIC_ void WM_UART_Init(WM_Type *dev)
{

}

__INLINE_STATIC_ void WM_UART_Output(WM_Type *dev, WM_OM_Enum om)
{

}


void WM_Init(WM_Type *dev)
{
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv == FW_Driver_Find("uart->dev"))
    {
        dev->Init = WM_UART_Init;
        dev->Output = WM_UART_Output;
    }
    else
    {
        dev->Init = WM_IO_Init;
        dev->Output = WM_IO_Output;
    }
    
    dev->Init(dev);
}

void WM_Output(WM_Type *dev, WM_OM_Enum om)
{
    dev->Output(dev, om);
}




#include "fw_debug.h"
#if MODULE_TEST && IO_DEVICE_TEST && GYY0709_TEST
#include "key.h"
#include "fw_delay.h"



/* 
 * f:基准频率
 * num: 采样点数
 * 时间向量:
 */
#define NUM    1000
static float Y1[NUM], Y2[NUM], Y3[NUM], Y4[NUM], Y5[NUM];
static float Y_SUM[NUM];
static float Trap[NUM];
//static float Comp[NUM * 250];

void Wave_Generate(u32 f)
{
    float inc;
    float y1, y2, y3, y4, y5;
    u16 i, j;
    float comp;
    
    inc = PI / NUM;
    
    for(i = 0; i < NUM; i++)
    {
        y1 = sin(inc * i);
        Y1[i] = y1;
        
        y2 = sin(2 * inc * i);
        Y2[i] = y2;
        
        y3 = sin(4 * inc * i);
        Y3[i] = y3;
        
        y4 = sin(6 * inc * i);
        Y4[i] = y4;
        
        y5 = sin(8 * inc * i);
        Y5[i] = y5;
        
        Y_SUM[i] = (y1 + y2 + y3 + y4 + y5) / 3 + 1;
        
//        printf("%f, %f, %f, %f, %f, %f\r\n", y1, y2, y3, y4, y5, Y_SUM[i]);
    }
    
    float slope = 1 / 0.05;
    for(i = 0; i < NUM / 5; i++)
    {
        Trap[i] = slope * (i * 0.25 / 1000);
    }
    
    for(i = NUM / 5; i < NUM * 4 / 5; i++)
    {
        Trap[i] = 1;
    }
    
    /*
    signal(t > (t_ramp_up + t_flat) & t <= (t_ramp_up + t_flat + t_ramp_down)) = ...
    slope_down * (t - (t_ramp_up + t_flat)) + slope_up * t_ramp_up;
    */
    for(i = NUM * 4 / 5; i < NUM; i++)
    {
        Trap[i] = -slope * ((i * 0.25 / 1000) - 0.05 - 0.15) + slope * 0.05;
    }
    
    for(i = 0; i < NUM / 50; i++)
    {
//        printf("%f\r\n", Trap[i]);
        for(j = 0; j < NUM; j++)
        {
            comp = Trap[j * 50] * Y_SUM[j];
            printf("%f\r\n", comp);
        }
    }
}


void Test(void)
{
    u16 VCC_EN = PA2;
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    Key_Type K2, K3;
    u8 kv2, kv3;
    
    Key_DeInit(&K2);
    K2.Pin = PB5;
    K2.Level = LEVEL_H;
    Key_Init(&K2);
    
    Key_DeInit(&K3);
    K3.Pin = PB6;
    K3.Level = LEVEL_H;
    Key_Init(&K3);
    
    WM_Type WM;
    WM.L1_Pin = PB15;
    WM.L2_Pin = PB14;
    WM.L3_Pin = PB13;
    WM_Init(&WM);
    
    u8 om = WM_OM_Stop;
    
    FW_UART_PrintInit(PA4, 9600);
//    Wave_Generate(250);
    
    while(1)
    {
        kv2 = Key_Get(&K2);
        if(kv2 == Key_State_Short)
        {
            WM_Output(&WM, (WM_OM_Enum)om);
        }
        
        kv3 = Key_Get(&K3);
        if(kv3 == Key_State_Short)
        {
            om++;
            if(om == 1)  om = 2;
            if(om > WM_OM_HH)  om = 0;
        }
        
        FW_Delay_Ms(10);
    }
}

/*
L1   L2    L3
0    0     x    0     x
0    1     0    2     ==         150ms总周期，1ms周期尖端脉冲
1    0     0    4     ==
1    1     0    6     
0    1     1    3     
1    0     1    5     
1    1     1    7     === ==
*/

#endif

