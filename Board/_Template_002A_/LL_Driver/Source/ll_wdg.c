#include "ll_include.h"

#include "fw_wdg.h"

/*
预分频系数  最小超时(ms)  最大超时(ms)
    4          0.125         512
    8          0.25          1024
    16         0.5           2048
    32         1             4096
    64         2             8192
    128        4             16384
    256        8             32768
*/

/* 内部独立看门狗底层驱动 */
__INLINE_STATIC_ void IWDG_Init(FW_WDG_Type *dev, u32 wdg_time)
{
    u8 prescaler;
    u16 reload;
    
    dev->WDGT_Uint = WDGT_UINT_MS;
    
    prescaler = 
    (wdg_time < 512) ? LL_IWDG_PRESCALER_4 :\
    (wdg_time < 1024 && wdg_time >= 512) ? LL_IWDG_PRESCALER_8 :\
    (wdg_time < 2048 && wdg_time >= 1024) ? LL_IWDG_PRESCALER_16 :\
    (wdg_time < 4096 && wdg_time >= 2048) ? LL_IWDG_PRESCALER_32 :\
    (wdg_time < 8192 && wdg_time >= 4096) ? LL_IWDG_PRESCALER_64 :\
    (wdg_time < 16384 && wdg_time >= 8192) ? LL_IWDG_PRESCALER_128 :\
    LL_IWDG_PRESCALER_256;
    
    while(1)
    {
        reload = wdg_time * 32 / (1 << (prescaler + 2));
        if(reload > 0xFFF)  prescaler += 1;
        else  break;
    }
    
    /* 使用LSI作为IWDG时钟源 */
    LL_RCC_LSI_Enable();
    while(LL_RCC_LSI_IsReady() != SET);
    
    LL_IWDG_Enable(LL_IWDG);
    LL_IWDG_EnableWriteAccess(LL_IWDG);
    LL_IWDG_SetPrescaler(LL_IWDG, prescaler);
    LL_IWDG_SetReloadCounter(LL_IWDG, reload);
    while(LL_IWDG_IsReady(LL_IWDG) == RESET);
    LL_IWDG_ReloadCounter(LL_IWDG);
    LL_IWDG_DisableWriteAccess(LL_IWDG);
}

__INLINE_STATIC_ void IWDG_FeedDog(FW_WDG_Type *dev)
{
    LL_IWDG_EnableWriteAccess(LL_IWDG);
    LL_IWDG_ReloadCounter(LL_IWDG);
    LL_IWDG_DisableWriteAccess(LL_IWDG);
}

__INLINE_STATIC_ u32  IWDG_Get_MaxTime(FW_WDG_Type *dev)
{
    return 32768;
}

__INLINE_STATIC_ u32  IWDG_Get_MinTime(FW_WDG_Type *dev)
{
    return 1;
}




const static WDG_Driver_Type IWDG_Driver =
{
    .Init = IWDG_Init,
    .FeedDog = IWDG_FeedDog,
    .Get_MaxTime = IWDG_Get_MaxTime,
    .Get_MinTime = IWDG_Get_MinTime,
};
FW_DRIVER_REGIST("ll->wdg", &IWDG_Driver, IWDG);




#if 0
/* 内部窗口看门狗底层驱动 */
void WWDG_IRQHandler(void)
{
    
}

__INLINE_STATIC_ void WWDG_Init(WDG_Type *dev, u32 wdg_time)
{

}

__INLINE_STATIC_ void WWDG_FeedDog(WDG_Type *dev)
{

}


#define WWDG_DRV_ID          FW_DRIVER_ID(FW_Device_WDG, WWDG_DRV_NUM)
const static WDG_Driver_Type WWDG_Driver =
{
    .Init = WWDG_Init,
    .FeedDog = WWDG_FeedDog,
};
FW_DRIVER_REGIST(WWDG_DRV_ID, &WWDG_Driver, WWDG);
#endif



#include "project_debug.h"
#if MODULE_TEST && WDG_TEST
#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_wdg.h"
#include "fw_delay.h"


//static FW_UART_Type *

void Test(void)
{
    FW_UART_Type *fd_uart;
    u8 msg, len;
    
    fd_uart = FW_Device_Find("huart1");
    if(fd_uart == NULL)
    {
        while(1);
    }
    
    fd_uart->TX_Pin = PA2;
    fd_uart->RX_Pin = PA3;
    fd_uart->Baudrate = 9600;
    fd_uart->Config.RX_Mode = TRM_INT;
    fd_uart->FIFO_RX_Buffer_Size = 10;
    FW_UART_Init(fd_uart);
    FW_UART_PrintInit(fd_uart);
    
    printf("wdg0\r\n");
    
    FW_IWDG_Init(5 * 1000);
    
    while(1)
    {
        len = FW_UART_Read(fd_uart, &msg, 1);
        if(len)
        {
            if(msg == 'F')
            {
                FW_IWDG_FeedDog();
            }
        }
        FW_Delay_Ms(100);
    }
}


#endif

