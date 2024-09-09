#include "ll_include.h"
#include "ll_system.h"

#include "fw_wdg.h"


/* 内部独立看门狗底层驱动 */
__INLINE_STATIC_ void IWDG_Init(FW_WDG_Type *dev, u32 wdg_time)
{
    u32 ticks;
    
    dev->WDGT_Uint = WDGT_UINT_MS;
    
    ticks = wdg_time * LL_Clocks.APBClk / 1000;
    
    WDT_Init(WDT, ticks, WDT_MODE_RESET);
    
    WDT_Start(WDT);
}

__INLINE_STATIC_ void IWDG_FeedDog(FW_WDG_Type *dev)
{
    WDT_Feed(WDT);
}

__INLINE_STATIC_ u32  IWDG_Get_MaxTime(FW_WDG_Type *dev)
{
    return (u32)(0xFFFFFFFF / (LL_Clocks.APBClk / 1000));
}

__INLINE_STATIC_ u32  IWDG_Get_MinTime(FW_WDG_Type *dev)
{
    return 1;
}


/*  */
__CONST_STATIC_ WDG_Driver_Type IWDG_Driver =
{
    .Init = IWDG_Init,
    .FeedDog = IWDG_FeedDog,
    .Get_MaxTime = IWDG_Get_MaxTime,
    .Get_MinTime = IWDG_Get_MinTime,
};
FW_DRIVER_REGIST("ll->iwdg", &IWDG_Driver, IWDG);


/* 内部窗口看门狗底层驱动 */
void WWDG_IRQHandler(void)
{
    
}

__INLINE_STATIC_ void WWDG_Init(FW_WDG_Type *dev, u32 wdg_time)
{

}

__INLINE_STATIC_ void WWDG_FeedDog(FW_WDG_Type *dev)
{

}


/*  */
__CONST_STATIC_ WDG_Driver_Type WWDG_Driver =
{
    .Init = WWDG_Init,
    .FeedDog = WWDG_FeedDog,
};
FW_DRIVER_REGIST("ll->wwdg", &WWDG_Driver, WWDG);

