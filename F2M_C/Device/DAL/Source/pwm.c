#include "pwm.h"




void PWM_Init(PWM_Type *dev)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

void PWM_Set_Frequency(PWM_Type *dev, u32 freq)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Set_Frequency(dev, freq);
}

void PWM_Set_DC(PWM_Type *dev, u16 dc)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    if(dc > PWM_DC_MAX)  dc = PWM_DC_MAX;
    drv->Set_DC(dev, dc);
}

void PWM_Set_PulseNum(PWM_Type *dev, u32 num)
{
    
}

void PWM_CTL(PWM_Type *dev, u8 state)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->CTL(dev, state);
}

