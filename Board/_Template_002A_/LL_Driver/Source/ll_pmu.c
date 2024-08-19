#include "ll_include.h"

#include "fw_pwr.h"


#if defined(PWR_MOD_EN) && PWR_MOD_EN


__INLINE_STATIC_ void PMU_Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
}

__INLINE_STATIC_ void PMU_SetLPM(FW_LPM_Enum lpm)
{
    if(lpm == FW_LPM_Sleep)
    {
        /* 进入Sleep模式，等待中断指令 */
        LL_LPM_EnableSleep();
        __WFI();
    }
    else if(lpm == FW_LPM_Stop)
    {
        LL_PWR_EnableLowPowerRunMode();
        LL_LPM_EnableDeepSleep();
        __WFI();
    }
    else
    {
    
    }
}




#define PWR_DRV_ID           FW_DRIVER_ID(FW_Device_PWR, PWR_DRV_NUM)
const static FW_PWR_Driver_Type PWR_Driver =
{
    .Init = PMU_Init,
    .SetLPM = PMU_SetLPM,
};
FW_DRIVER_REGIST("ll->pmu", &PWR_Driver, PWR);


#endif  /* defined(PWR_MOD_EN) && PWR_MOD_EN */

