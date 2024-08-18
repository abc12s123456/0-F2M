//#include "ll_include.h"

//#include "fw_wdg.h"


///* 内部独立看门狗底层驱动 */
//__INLINE_STATIC_ void IWDG_Init(FW_WDG_Type *dev, u32 wdg_time)
//{
//    u8 prescaler;
//    u16 reload;
//    
//    dev->WDGT_Uint = WDGT_UINT_MS;
//    
//    prescaler = 
//    (wdg_time < 409)                       ? FWDGT_PSC_DIV4 :\
//    (wdg_time < 819 && wdg_time >= 409)    ? FWDGT_PSC_DIV8 :\
//    (wdg_time < 1638 && wdg_time >= 819)   ? FWDGT_PSC_DIV16 :\
//    (wdg_time < 3276 && wdg_time >= 1638)  ? FWDGT_PSC_DIV32 :\
//    (wdg_time < 6553 && wdg_time >= 3276)  ? FWDGT_PSC_DIV64 :\
//    (wdg_time < 13107 && wdg_time >= 6553) ? FWDGT_PSC_DIV128 :\
//    FWDGT_PSC_DIV256;
//    
//    while(1)
//    {
//        reload = wdg_time * 40 / (1 << (prescaler + 2));
//        if(reload > 0xFFF)  prescaler += 1;
//        else  break;
//    }
//    
//    fwdgt_write_enable();
//    fwdgt_config(reload, prescaler);
//    fwdgt_counter_reload();
//    fwdgt_write_disable();
//    fwdgt_enable();
//}

//__INLINE_STATIC_ void IWDG_FeedDog(FW_WDG_Type *dev)
//{
//    fwdgt_write_enable();
//    fwdgt_counter_reload();
//    fwdgt_write_disable();
//}

//__INLINE_STATIC_ u32  IWDG_Get_MaxTime(FW_WDG_Type *dev)
//{
//    return 26214;
//}

//__INLINE_STATIC_ u32  IWDG_Get_MinTime(FW_WDG_Type *dev)
//{
//    return 1;
//}


///*  */
//__CONST_STATIC_ WDG_Driver_Type IWDG_Driver =
//{
//    .Init = IWDG_Init,
//    .FeedDog = IWDG_FeedDog,
//    .Get_MaxTime = IWDG_Get_MaxTime,
//    .Get_MinTime = IWDG_Get_MinTime,
//};
//FW_DRIVER_REGIST("ll->iwdg", &IWDG_Driver, IWDG);


///* 内部窗口看门狗底层驱动 */
//void WWDG_IRQHandler(void)
//{
//    
//}

//__INLINE_STATIC_ void WWDG_Init(FW_WDG_Type *dev, u32 wdg_time)
//{

//}

//__INLINE_STATIC_ void WWDG_FeedDog(FW_WDG_Type *dev)
//{

//}


///*  */
//__CONST_STATIC_ WDG_Driver_Type WWDG_Driver =
//{
//    .Init = WWDG_Init,
//    .FeedDog = WWDG_FeedDog,
//};
//FW_DRIVER_REGIST("ll->wwdg", &WWDG_Driver, WWDG);

