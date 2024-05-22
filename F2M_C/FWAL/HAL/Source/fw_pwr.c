#include "fw_pwr.h"
#include "fw_debug.h"


static FW_PWR_Driver_Type *Driver;
static FW_PWR_Type PWR;


extern void FW_Lock(void);
extern void FW_Unlock(void);


void FW_PWR_DeInit(void)
{
    FW_PWR_Type *dev = &PWR;
    
    dev->IH_CB = NULL;
    dev->IH_Pdata = NULL;
    dev->LVDT = 0;
}

void FW_PWR_Init(void)
{
    Driver = FW_Driver_Find("ll->pwr");
    if(Driver == NULL)
    {
        /* pwr driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->Init);
    Driver->Init();
}

void FW_PWR_SetLPM(FW_LPM_Enum lpm)
{
    FW_ASSERT(Driver->SetLPM);
    Driver->SetLPM(lpm);
}

FW_PWR_Type *FW_PWR_GetDevice(void)
{
    return &PWR;
}

void FW_PWR_SetLVDT(FW_LVDT_Enum lvdt)
{
    FW_PWR_Type *dev = &PWR;
    dev->LVDT = lvdt;
}

void FW_PWR_SetCB(void (*cb)(void *), void *pdata)
{
    FW_PWR_Type *dev = &PWR;
    FW_Lock();
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
    FW_Unlock();
}

void FW_PWR_IH(void)
{
    FW_PWR_Type *dev = &PWR;
    if(dev->IH_CB)  dev->IH_CB(dev->IH_Pdata);
}

